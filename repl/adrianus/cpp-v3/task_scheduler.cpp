#include "task_scheduler.h"

#include "stack_thread.cpp"

#include "gc.h"
#include "objects.h"
#include "stack_thread.h"

#include <stdexcept>
#include <thread>
#include <utility>

thread_local bool TaskScheduler::tls_in_worker_{false};
thread_local AdTaskExecutionContext* g_tls_task_ctx{nullptr};

static std::shared_ptr<TaskScheduler> g_ad_global_scheduler;

void ad_set_global_task_scheduler(std::shared_ptr<TaskScheduler> s) {
    g_ad_global_scheduler = std::move(s);
}

std::shared_ptr<TaskScheduler> ad_global_task_scheduler() {
    return g_ad_global_scheduler;
}

bool ad_task_status_terminal(AdTaskStatus status) {
    return status == AdTaskStatus::Completed || status == AdTaskStatus::Failed || status == AdTaskStatus::Cancelled;
}

const char* ad_task_status_name(AdTaskStatus s) {
    switch (s) {
        case AdTaskStatus::Ready:
            return "ready";
        case AdTaskStatus::Running:
            return "running";
        case AdTaskStatus::Yielded:
            return "yielded";
        case AdTaskStatus::Waiting:
            return "waiting";
        case AdTaskStatus::Completed:
            return "completed";
        case AdTaskStatus::Failed:
            return "failed";
        case AdTaskStatus::Cancelled:
            return "cancelled";
    }
    return "unknown";
}

void ad_tls_set_task_ctx(AdTaskExecutionContext* ctx) { g_tls_task_ctx = ctx; }

void ad_tls_clear_task_ctx(AdTaskExecutionContext* prev) { g_tls_task_ctx = prev; }

AdTaskExecutionContext* ad_tls_task_ctx() { return g_tls_task_ctx; }

void ad_task_checkpoint() {
    AdTaskExecutionContext* ctx = g_tls_task_ctx;
    if (ctx == nullptr || !ctx->task) {
        return;
    }
    ctx->checkpoints++;
    if (ctx->task->cancel_requested.load()) {
        throw std::runtime_error("task cancelled");
    }
    if (ctx->remaining_budget > 0) {
        ctx->remaining_budget--;
    }
    if (ctx->remaining_budget == 0) {
        ctx->yield_requested = true;
        ctx->remaining_budget = ctx->checkpoint_budget;
        std::this_thread::yield();
    }
}

namespace {

Ad_Object* copy_task_result(Ad_Object* result, GarbageCollector* gc) {
    if (result == nullptr || result == &NULLOBJECT) {
        return &NULLOBJECT;
    }
    if (gc != nullptr) {
        Ad_Object* copied = result->copy(gc);
        if (copied != nullptr) {
            return copied;
        }
    }
    return result;
}

}  // namespace

Ad_Object* ad_task_join_handle(const std::shared_ptr<AdTaskHandle>& h, GarbageCollector* gc) {
    if (!h) {
        throw std::runtime_error("join on null task");
    }
    if (h->joined.exchange(true)) {
        throw std::runtime_error("join on already joined task");
    }

    Ad_Object* ready_value = nullptr;
    std::exception_ptr ready_error;
    bool already_done = false;
    {
        std::lock_guard<std::mutex> lk(h->mu);
        if (h->has_result) {
            already_done = true;
            ready_value = h->result_value;
            ready_error = h->result_error;
        }
    }
    if (already_done) {
        if (h->has_overflow_pthread) {
            pthread_join(h->overflow_pthread, nullptr);
            h->has_overflow_pthread = false;
        }
        if (ready_error) {
            std::rethrow_exception(ready_error);
        }
        return copy_task_result(ready_value, gc);
    }

    if (ad_tls_task_ctx() != nullptr) {
        AdSuspendRequest sr;
        sr.kind = AdSuspendRequest::Kind::Join;
        sr.join_target = h;
        sr.resume_cont = [](Ad_Object* v) { return v; };
        throw sr;
    }

    try {
        Ad_Object* result = h->future.get();
        if (h->has_overflow_pthread) {
            pthread_join(h->overflow_pthread, nullptr);
            h->has_overflow_pthread = false;
        }
        return copy_task_result(result, gc);
    } catch (...) {
        if (h->has_overflow_pthread) {
            pthread_join(h->overflow_pthread, nullptr);
            h->has_overflow_pthread = false;
        }
        throw;
    }
}

namespace {

size_t default_workers() {
    unsigned hc = std::thread::hardware_concurrency();
    if (hc == 0) {
        return 4;
    }
    return hc < 2 ? 2 : static_cast<size_t>(hc);
}

}  // namespace

void* TaskScheduler::worker_entry(void* self) {
    static_cast<TaskScheduler*>(self)->workerLoop();
    return nullptr;
}

void* TaskScheduler::timer_entry(void* self) {
    static_cast<TaskScheduler*>(self)->timerLoop();
    return nullptr;
}

TaskScheduler::TaskScheduler(size_t num_workers) {
    num_workers_ = num_workers == 0 ? default_workers() : num_workers;
    workers_.resize(num_workers_);
    constexpr size_t kStack = 8u * 1024u * 1024u;
    for (size_t i = 0; i < num_workers_; ++i) {
        if (!start_pthread_with_stack(kStack, worker_entry, this, &workers_[i])) {
            throw std::runtime_error("failed to start worker thread");
        }
    }
    if (!start_pthread_with_stack(kStack, timer_entry, this, &timer_thread_)) {
        throw std::runtime_error("failed to start timer thread");
    }
    timer_started_ = true;
}

TaskScheduler::~TaskScheduler() {
    std::vector<std::shared_ptr<AdScheduledTask>> to_fail;
    {
        std::lock_guard<std::mutex> lk(mu_);
        stop_ = true;
        while (!queue_.empty()) {
            to_fail.push_back(std::move(queue_.front()));
            queue_.pop();
        }
    }
    {
        std::lock_guard<std::mutex> lk(delay_mu_);
        while (!delay_queue_.empty()) {
            to_fail.push_back(delay_queue_.top().task);
            delay_queue_.pop();
        }
    }
    for (auto& t : to_fail) {
        if (!t || !t->handle) {
            continue;
        }
        if (ad_task_status_terminal(t->handle->status.load())) {
            continue;
        }
        try {
            failTask(t, AdTaskStatus::Cancelled);
        } catch (...) {
        }
    }
    delay_cv_.notify_all();
    cv_.notify_all();
    if (timer_started_) {
        pthread_join(timer_thread_, nullptr);
    }
    for (pthread_t w : workers_) {
        pthread_join(w, nullptr);
    }
}

bool TaskScheduler::in_worker_thread() { return tls_in_worker_; }

void TaskScheduler::set_quantum_budget(size_t budget) {
    if (budget == 0) {
        throw std::runtime_error("scheduler quantum budget must be > 0");
    }
    quantum_budget_.store(budget);
}

void TaskScheduler::enqueueTask(std::shared_ptr<AdScheduledTask> task) {
    {
        std::lock_guard<std::mutex> lk(mu_);
        if (stop_) {
            return;
        }
        queue_.push(std::move(task));
    }
    cv_.notify_one();
}

void TaskScheduler::finishTask(const std::shared_ptr<AdScheduledTask>& task, Ad_Object* value, AdTaskStatus status) {
    std::vector<std::shared_ptr<AdScheduledTask>> waiters;
    {
        std::lock_guard<std::mutex> lk(task->handle->mu);
        task->handle->result_value = value;
        task->handle->result_error = nullptr;
        task->handle->has_result = true;
        task->handle->status.store(status);
        task->handle->metrics.completed_at = std::chrono::steady_clock::now();
        task->handle->parked_scheduled.reset();
        waiters.swap(task->handle->waiters);
    }
    try {
        task->promise->set_value(value);
    } catch (...) {
    }
    for (auto& w : waiters) {
        if (!w || !w->handle) {
            continue;
        }
        if (ad_task_status_terminal(w->handle->status.load())) {
            continue;
        }
        if (w->handle->cancel_requested.load()) {
            failTask(w, AdTaskStatus::Cancelled);
            continue;
        }
        w->pending_resume = value;
        w->pending_error = nullptr;
        w->has_pending_resume = true;
        {
            std::lock_guard<std::mutex> lk(w->handle->mu);
            w->handle->parked_scheduled.reset();
        }
        enqueueTask(w);
    }
}

void TaskScheduler::failTask(const std::shared_ptr<AdScheduledTask>& task, AdTaskStatus status) {
    std::exception_ptr err;
    if (status == AdTaskStatus::Cancelled) {
        err = std::make_exception_ptr(std::runtime_error("task cancelled"));
    } else {
        err = std::current_exception();
        if (!err) {
            err = std::make_exception_ptr(std::runtime_error("task failed"));
        }
    }
    std::vector<std::shared_ptr<AdScheduledTask>> waiters;
    {
        std::lock_guard<std::mutex> lk(task->handle->mu);
        if (ad_task_status_terminal(task->handle->status.load()) && task->handle->has_result) {
            return;
        }
        task->handle->result_value = nullptr;
        task->handle->result_error = err;
        task->handle->has_result = true;
        task->handle->status.store(status);
        task->handle->metrics.completed_at = std::chrono::steady_clock::now();
        task->handle->parked_scheduled.reset();
        waiters.swap(task->handle->waiters);
    }
    try {
        task->promise->set_exception(err);
    } catch (...) {
    }
    for (auto& w : waiters) {
        if (!w || !w->handle) {
            continue;
        }
        if (ad_task_status_terminal(w->handle->status.load())) {
            continue;
        }
        if (w->handle->cancel_requested.load()) {
            failTask(w, AdTaskStatus::Cancelled);
            continue;
        }
        w->pending_resume = nullptr;
        w->pending_error = err;
        w->has_pending_resume = true;
        {
            std::lock_guard<std::mutex> lk(w->handle->mu);
            w->handle->parked_scheduled.reset();
        }
        enqueueTask(w);
    }
}

void TaskScheduler::armResumeSlice(std::shared_ptr<AdScheduledTask> task,
                                   std::function<Ad_Object*(Ad_Object*)> resume_cont) {
    std::shared_ptr<AdScheduledTask> st = std::move(task);
    st->run_slice = [st, resume_cont = std::move(resume_cont)](const std::shared_ptr<AdTaskHandle>& handle,
                                                               size_t budget) mutable {
        Ad_Object* pending = st->pending_resume;
        std::exception_ptr pending_error = st->pending_error;
        st->has_pending_resume = false;
        st->pending_error = nullptr;
        st->pending_resume = nullptr;
        if (handle->frontend_kind == AdTaskFrontendKind::Vm) {
            return ad_vm_resume_slice(handle, budget, resume_cont, pending, pending_error);
        }
        return ad_eval_resume_slice(handle, budget, resume_cont, pending, pending_error);
    };
}

void TaskScheduler::parkOnDelay(std::shared_ptr<AdScheduledTask> task,
                                std::chrono::steady_clock::time_point wake_at) {
    task->handle->status.store(AdTaskStatus::Waiting);
    uint64_t gen = 0;
    {
        std::lock_guard<std::mutex> lk(task->handle->mu);
        task->handle->delay_generation++;
        gen = task->handle->delay_generation;
        task->handle->parked_scheduled = task;
    }
    {
        std::lock_guard<std::mutex> lk(delay_mu_);
        delay_queue_.push(DelayItem{wake_at, gen, task});
    }
    delay_cv_.notify_one();
}

void TaskScheduler::parkOnJoin(std::shared_ptr<AdScheduledTask> task, const std::shared_ptr<AdTaskHandle>& target) {
    if (!target) {
        failTask(task, AdTaskStatus::Failed);
        return;
    }
    Ad_Object* ready_value = nullptr;
    std::exception_ptr ready_error;
    bool already_done = false;
    {
        std::lock_guard<std::mutex> lk(target->mu);
        if (target->has_result) {
            already_done = true;
            ready_value = target->result_value;
            ready_error = target->result_error;
        } else {
            task->handle->status.store(AdTaskStatus::Waiting);
            {
                std::lock_guard<std::mutex> lk2(task->handle->mu);
                task->handle->parked_scheduled = task;
            }
            target->waiters.push_back(task);
        }
    }
    if (already_done) {
        if (task->handle->cancel_requested.load()) {
            failTask(task, AdTaskStatus::Cancelled);
            return;
        }
        task->pending_resume = ready_value;
        task->pending_error = ready_error;
        task->has_pending_resume = true;
        enqueueTask(std::move(task));
    }
}

bool TaskScheduler::cancelParkedLocked(const std::shared_ptr<AdTaskHandle>& handle,
                                       std::vector<std::shared_ptr<AdScheduledTask>>* out_fail) {
    auto st = handle->status.load();
    if (st != AdTaskStatus::Waiting) {
        return false;
    }
    auto parked = handle->parked_scheduled.lock();
    if (!parked) {
        return false;
    }
    handle->delay_generation++;
    handle->parked_scheduled.reset();
    out_fail->push_back(parked);
    return true;
}

void TaskScheduler::requestCancel(const std::shared_ptr<AdTaskHandle>& handle) {
    if (!handle) {
        return;
    }
    handle->cancel_requested.store(true);
    std::vector<std::shared_ptr<AdScheduledTask>> to_fail;
    {
        std::lock_guard<std::mutex> lk(handle->mu);
        cancelParkedLocked(handle, &to_fail);
    }
    for (auto& t : to_fail) {
        failTask(t, AdTaskStatus::Cancelled);
    }
    delay_cv_.notify_all();
}

void TaskScheduler::timerLoop() {
    while (true) {
        std::shared_ptr<AdScheduledTask> due;
        {
            std::unique_lock<std::mutex> lk(delay_mu_);
            if (stop_ && delay_queue_.empty()) {
                break;
            }
            if (delay_queue_.empty()) {
                delay_cv_.wait(lk, [&] { return stop_ || !delay_queue_.empty(); });
                if (stop_ && delay_queue_.empty()) {
                    break;
                }
                continue;
            }
            auto wake = delay_queue_.top().wake_at;
            auto now = std::chrono::steady_clock::now();
            if (wake > now) {
                delay_cv_.wait_until(lk, wake, [&] {
                    return stop_ || delay_queue_.empty() ||
                           delay_queue_.top().wake_at <= std::chrono::steady_clock::now();
                });
                if (stop_ && delay_queue_.empty()) {
                    break;
                }
                continue;
            }
            DelayItem item = delay_queue_.top();
            delay_queue_.pop();
            due = item.task;
            if (due && due->handle) {
                std::lock_guard<std::mutex> hlk(due->handle->mu);
                if (item.generation != due->handle->delay_generation) {
                    due.reset();
                } else if (ad_task_status_terminal(due->handle->status.load())) {
                    due.reset();
                }
            }
        }
        if (!due) {
            continue;
        }
        if (due->handle->cancel_requested.load()) {
            failTask(due, AdTaskStatus::Cancelled);
            continue;
        }
        due->pending_resume = nullptr;
        due->pending_error = nullptr;
        due->has_pending_resume = true;
        {
            std::lock_guard<std::mutex> lk(due->handle->mu);
            due->handle->parked_scheduled.reset();
        }
        enqueueTask(std::move(due));
    }
}

void TaskScheduler::workerLoop() {
    tls_in_worker_ = true;
    while (true) {
        std::shared_ptr<AdScheduledTask> task;
        {
            std::unique_lock<std::mutex> lk(mu_);
            cv_.wait(lk, [&] { return stop_ || !queue_.empty(); });
            if (stop_ && queue_.empty()) {
                break;
            }
            if (!queue_.empty()) {
                task = std::move(queue_.front());
                queue_.pop();
            }
        }
        if (!task) {
            continue;
        }
        if (ad_task_status_terminal(task->handle->status.load())) {
            continue;
        }
        if (task->handle->cancel_requested.load()) {
            failTask(task, AdTaskStatus::Cancelled);
            continue;
        }
        if (task->handle->metrics.run_slices == 0) {
            task->handle->metrics.first_started_at = std::chrono::steady_clock::now();
        }
        task->handle->status.store(AdTaskStatus::Running);
        task->handle->metrics.run_slices++;

        try {
            AdRunSliceResult result = task->run_slice(task->handle, quantum_budget_.load());
            task->handle->metrics.checkpoint_count += result.checkpoints;
            if (result.status == AdRunSliceResult::Status::Yielded && result.continuation) {
                task->handle->status.store(AdTaskStatus::Yielded);
                task->handle->metrics.yield_count++;
                task->handle->continuation = std::make_shared<AdTaskContinuationState>();
                task->handle->continuation->frame_id = task->handle->metrics.run_slices;
                auto continuation = std::move(result.continuation);
                task->run_slice = [cont = std::move(continuation)](const std::shared_ptr<AdTaskHandle>& handle,
                                                                   size_t budget) mutable {
                    return cont(handle, budget);
                };
                enqueueTask(std::move(task));
            } else if (result.status == AdRunSliceResult::Status::Suspended && result.resume_continuation) {
                if (task->handle->cancel_requested.load()) {
                    failTask(task, AdTaskStatus::Cancelled);
                } else {
                    armResumeSlice(task, std::move(result.resume_continuation));
                    if (result.suspend_kind == AdSuspendRequest::Kind::Delay) {
                        parkOnDelay(std::move(task), result.wake_at);
                    } else {
                        parkOnJoin(std::move(task), result.join_target);
                    }
                }
            } else {
                task->handle->continuation.reset();
                finishTask(task, result.value, AdTaskStatus::Completed);
            }
        } catch (...) {
            failTask(task, AdTaskStatus::Failed);
        }
    }
    tls_in_worker_ = false;
}

std::shared_ptr<AdTaskHandle> TaskScheduler::submitPreemptible(
    std::function<AdRunSliceResult(const std::shared_ptr<AdTaskHandle>&, size_t)> run_slice) {
    auto prom = std::make_shared<std::promise<Ad_Object*>>();
    std::future<Ad_Object*> fut = prom->get_future();
    auto handle = std::make_shared<AdTaskHandle>();
    handle->future = std::move(fut);
    handle->status.store(AdTaskStatus::Ready);
    handle->scheduler = shared_from_this();

    auto scheduled = std::make_shared<AdScheduledTask>();
    scheduled->handle = handle;
    scheduled->promise = prom;
    scheduled->run_slice = std::move(run_slice);

    enqueueTask(std::move(scheduled));
    return handle;
}

std::shared_ptr<AdTaskHandle> TaskScheduler::submit(std::function<Ad_Object*()> work) {
    return submitPreemptible([w = std::move(work)](const std::shared_ptr<AdTaskHandle>&, size_t) mutable {
        AdRunSliceResult result;
        result.status = AdRunSliceResult::Status::Completed;
        result.value = w();
        return result;
    });
}
