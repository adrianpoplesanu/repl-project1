#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <string>
#include <vector>

class Ad_Object;
class GarbageCollector;

enum class AdTaskStatus {
    Ready,
    Running,
    Yielded,
    Waiting,
    Completed,
    Failed,
    Cancelled,
};

enum class AdTaskFrontendKind {
    None,
    Eval,
    Vm,
};

struct AdTaskContinuationState {
    uint64_t frame_id{0};
};

struct AdTaskMetrics {
    std::chrono::steady_clock::time_point submitted_at{std::chrono::steady_clock::now()};
    std::chrono::steady_clock::time_point first_started_at{};
    std::chrono::steady_clock::time_point completed_at{};
    uint64_t run_slices{0};
    uint64_t yield_count{0};
    uint64_t checkpoint_count{0};
};

struct AdScheduledTask;

struct AdTaskHandle {
    std::future<Ad_Object*> future;
    pthread_t overflow_pthread{};
    bool has_overflow_pthread{false};
    // Single-shot, matching lang4: a second join/await throws. Not Kotlin multi-await.
    std::atomic<bool> joined{false};
    std::atomic<AdTaskStatus> status{AdTaskStatus::Ready};
    std::atomic<bool> cancel_requested{false};
    std::shared_ptr<AdTaskContinuationState> continuation;
    AdTaskMetrics metrics;

    std::mutex mu;
    std::vector<std::shared_ptr<AdScheduledTask>> waiters;
    Ad_Object* result_value{nullptr};
    std::exception_ptr result_error;
    bool has_result{false};

    std::weak_ptr<class TaskScheduler> scheduler;
    std::weak_ptr<AdScheduledTask> parked_scheduled;
    uint64_t delay_generation{0};

    // Parent-heap objects the task body uses. Marked from OBJ_TASK so a parent
    // collection cannot free a closure or argument while the task is parked.
    std::vector<Ad_Object*> kept_alive;

    // Keeps the per-task heap (and VM, for the bytecode frontend) alive across park.
    AdTaskFrontendKind frontend_kind{AdTaskFrontendKind::None};
    std::shared_ptr<void> frontend_state;
};

// Not a std::exception. Must propagate through catch (const std::exception&).
struct AdSuspendRequest {
    enum class Kind { Delay, Join };

    Kind kind{Kind::Delay};
    std::chrono::steady_clock::time_point wake_at{};
    std::shared_ptr<AdTaskHandle> join_target;
    std::function<Ad_Object*(Ad_Object*)> resume_cont;
    bool done_after{false};

    const char* what() const noexcept { return "task suspended"; }
};

struct AdRunSliceResult {
    enum class Status { Completed, Yielded, Suspended };
    Status status{Status::Completed};
    Ad_Object* value{nullptr};
    std::function<AdRunSliceResult(const std::shared_ptr<AdTaskHandle>&, size_t)> continuation;
    std::function<Ad_Object*(Ad_Object*)> resume_continuation;
    AdSuspendRequest::Kind suspend_kind{AdSuspendRequest::Kind::Delay};
    std::chrono::steady_clock::time_point wake_at{};
    std::shared_ptr<AdTaskHandle> join_target;
    uint64_t checkpoints{0};
};

struct AdScheduledTask {
    std::shared_ptr<AdTaskHandle> handle;
    std::shared_ptr<std::promise<Ad_Object*>> promise;
    std::function<AdRunSliceResult(const std::shared_ptr<AdTaskHandle>&, size_t)> run_slice;
    Ad_Object* pending_resume{nullptr};
    std::exception_ptr pending_error;
    bool has_pending_resume{false};
};

class TaskScheduler : public std::enable_shared_from_this<TaskScheduler> {
 public:
    explicit TaskScheduler(size_t num_workers);
    ~TaskScheduler();

    std::shared_ptr<AdTaskHandle> submit(std::function<Ad_Object*()> work);
    std::shared_ptr<AdTaskHandle> submitPreemptible(
        std::function<AdRunSliceResult(const std::shared_ptr<AdTaskHandle>&, size_t)> run_slice);
    size_t quantum_budget() const { return quantum_budget_.load(); }
    void set_quantum_budget(size_t budget);

    void requestCancel(const std::shared_ptr<AdTaskHandle>& handle);

    static bool in_worker_thread();

 private:
    struct DelayItem {
        std::chrono::steady_clock::time_point wake_at;
        uint64_t generation{0};
        std::shared_ptr<AdScheduledTask> task;
    };

    struct DelayItemCompare {
        bool operator()(const DelayItem& a, const DelayItem& b) const {
            return a.wake_at > b.wake_at;
        }
    };

    static void* worker_entry(void* self);
    static void* timer_entry(void* self);
    void workerLoop();
    void timerLoop();
    void finishTask(const std::shared_ptr<AdScheduledTask>& task, Ad_Object* value, AdTaskStatus status);
    void failTask(const std::shared_ptr<AdScheduledTask>& task, AdTaskStatus status);
    void enqueueTask(std::shared_ptr<AdScheduledTask> task);
    void parkOnDelay(std::shared_ptr<AdScheduledTask> task, std::chrono::steady_clock::time_point wake_at);
    void parkOnJoin(std::shared_ptr<AdScheduledTask> task, const std::shared_ptr<AdTaskHandle>& target);
    void armResumeSlice(std::shared_ptr<AdScheduledTask> task,
                        std::function<Ad_Object*(Ad_Object*)> resume_cont);
    bool cancelParkedLocked(const std::shared_ptr<AdTaskHandle>& handle,
                            std::vector<std::shared_ptr<AdScheduledTask>>* out_fail);

    size_t num_workers_{0};
    std::vector<pthread_t> workers_;
    pthread_t timer_thread_{};
    bool timer_started_{false};

    std::mutex mu_;
    std::condition_variable cv_;
    std::queue<std::shared_ptr<AdScheduledTask>> queue_;
    bool stop_{false};
    std::atomic<size_t> quantum_budget_{20000};

    std::mutex delay_mu_;
    std::condition_variable delay_cv_;
    std::priority_queue<DelayItem, std::vector<DelayItem>, DelayItemCompare> delay_queue_;

    thread_local static bool tls_in_worker_;
};

struct AdTaskExecutionContext {
    std::shared_ptr<AdTaskHandle> task;
    size_t checkpoint_budget{0};
    size_t remaining_budget{0};
    bool yield_requested{false};
    uint64_t checkpoints{0};
};

const char* ad_task_status_name(AdTaskStatus s);
bool ad_task_status_terminal(AdTaskStatus status);

void ad_tls_set_task_ctx(AdTaskExecutionContext* ctx);
void ad_tls_clear_task_ctx(AdTaskExecutionContext* prev);
AdTaskExecutionContext* ad_tls_task_ctx();
void ad_task_checkpoint();

// Blocking on the caller when not inside a task. Parks (throws AdSuspendRequest) when it is.
Ad_Object* ad_task_join_handle(const std::shared_ptr<AdTaskHandle>& h, GarbageCollector* gc);

void ad_set_global_task_scheduler(std::shared_ptr<TaskScheduler> s);
std::shared_ptr<TaskScheduler> ad_global_task_scheduler();

AdRunSliceResult ad_eval_resume_slice(const std::shared_ptr<AdTaskHandle>& handle, size_t budget,
                                      const std::function<Ad_Object*(Ad_Object*)>& resume_cont,
                                      Ad_Object* pending, std::exception_ptr pending_error);

AdRunSliceResult ad_vm_resume_slice(const std::shared_ptr<AdTaskHandle>& handle, size_t budget,
                                    const std::function<Ad_Object*(Ad_Object*)>& resume_cont,
                                    Ad_Object* pending, std::exception_ptr pending_error);
