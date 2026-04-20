#include "task_scheduler.h"

#include "stack_thread.cpp"

#include "gc.h"
#include "objects.h"
#include "stack_thread.h"

#include <stdexcept>
#include <thread>

thread_local bool TaskScheduler::tls_in_worker_{false};
thread_local AdTaskExecutionContext* g_tls_task_ctx{nullptr};

static std::shared_ptr<TaskScheduler> g_ad_global_scheduler;

void ad_set_global_task_scheduler(std::shared_ptr<TaskScheduler> s) {
    g_ad_global_scheduler = std::move(s);
}

std::shared_ptr<TaskScheduler> ad_global_task_scheduler() {
    return g_ad_global_scheduler;
}

const char* ad_task_status_name(AdTaskStatus s) {
    switch (s) {
        case AdTaskStatus::Ready:
            return "ready";
        case AdTaskStatus::Running:
            return "running";
        case AdTaskStatus::Yielded:
            return "yielded";
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

Ad_Object* ad_task_join_handle(const std::shared_ptr<AdTaskHandle>& h, GarbageCollector* gc) {
    if (!h) {
        throw std::runtime_error("join on null task");
    }
    if (h->joined.exchange(true)) {
        throw std::runtime_error("join on already joined task");
    }
    try {
        Ad_Object* result = h->future.get();
        if (h->has_overflow_pthread) {
            pthread_join(h->overflow_pthread, nullptr);
            h->has_overflow_pthread = false;
        }
        if (result == nullptr) {
            return &NULLOBJECT;
        }
        if (gc != nullptr) {
            Ad_Object* copied = result->copy(gc);
            if (copied != nullptr) {
                return copied;
            }
        }
        return result;
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

void* run_std_function_void(void* p) {
    auto* f = static_cast<std::function<void()>*>(p);
    (*f)();
    delete f;
    return nullptr;
}

}  // namespace

void* TaskScheduler::worker_entry(void* self) {
    static_cast<TaskScheduler*>(self)->workerLoop();
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
}

TaskScheduler::~TaskScheduler() {
    {
        std::lock_guard<std::mutex> lk(mu_);
        stop_ = true;
    }
    cv_.notify_all();
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
        queue_.push(std::move(task));
    }
    cv_.notify_one();
}

void TaskScheduler::finishTask(const std::shared_ptr<AdScheduledTask>& task, Ad_Object* value, AdTaskStatus status) {
    task->handle->status.store(status);
    task->handle->metrics.completed_at = std::chrono::steady_clock::now();
    task->promise->set_value(value);
}

void TaskScheduler::failTask(const std::shared_ptr<AdScheduledTask>& task, AdTaskStatus status) {
    task->handle->status.store(status);
    task->handle->metrics.completed_at = std::chrono::steady_clock::now();
    if (status == AdTaskStatus::Cancelled) {
        task->promise->set_exception(std::make_exception_ptr(std::runtime_error("task cancelled")));
        return;
    }
    task->promise->set_exception(std::current_exception());
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

    auto scheduled = std::make_shared<AdScheduledTask>();
    scheduled->handle = handle;
    scheduled->promise = prom;
    scheduled->run_slice = std::move(run_slice);

    if (tls_in_worker_) {
        auto fp = new std::function<void()>([scheduled, this]() {
            try {
                for (;;) {
                    if (scheduled->handle->cancel_requested.load()) {
                        failTask(scheduled, AdTaskStatus::Cancelled);
                        return;
                    }
                    if (scheduled->handle->metrics.run_slices == 0) {
                        scheduled->handle->metrics.first_started_at = std::chrono::steady_clock::now();
                    }
                    scheduled->handle->status.store(AdTaskStatus::Running);
                    scheduled->handle->metrics.run_slices++;
                    AdRunSliceResult result = scheduled->run_slice(scheduled->handle, quantum_budget_.load());
                    scheduled->handle->metrics.checkpoint_count += result.checkpoints;
                    if (result.status == AdRunSliceResult::Status::Yielded && result.continuation) {
                        scheduled->handle->status.store(AdTaskStatus::Yielded);
                        scheduled->handle->metrics.yield_count++;
                        scheduled->handle->continuation = std::make_shared<AdTaskContinuationState>();
                        scheduled->handle->continuation->frame_id = scheduled->handle->metrics.run_slices;
                        auto continuation = std::move(result.continuation);
                        scheduled->run_slice =
                            [cont = std::move(continuation)](const std::shared_ptr<AdTaskHandle>& h,
                                                             size_t budget) mutable { return cont(h, budget); };
                        continue;
                    }
                    scheduled->handle->continuation.reset();
                    finishTask(scheduled, result.value, AdTaskStatus::Completed);
                    return;
                }
            } catch (...) {
                failTask(scheduled, AdTaskStatus::Failed);
            }
        });
        pthread_t tid{};
        constexpr size_t kStack = 8u * 1024u * 1024u;
        if (!start_pthread_with_stack(kStack, run_std_function_void, fp, &tid)) {
            delete fp;
            throw std::runtime_error("failed to start overflow thread");
        }
        handle->overflow_pthread = tid;
        handle->has_overflow_pthread = true;
        return handle;
    }

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
