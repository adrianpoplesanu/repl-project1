#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
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
    Completed,
    Failed,
    Cancelled,
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

struct AdTaskHandle {
    std::future<Ad_Object*> future;
    pthread_t overflow_pthread{};
    bool has_overflow_pthread{false};
    std::atomic<bool> joined{false};
    std::atomic<AdTaskStatus> status{AdTaskStatus::Ready};
    std::atomic<bool> cancel_requested{false};
    std::shared_ptr<AdTaskContinuationState> continuation;
    AdTaskMetrics metrics;
};

struct AdRunSliceResult {
    enum class Status { Completed, Yielded };
    Status status{Status::Completed};
    Ad_Object* value{nullptr};
    std::function<AdRunSliceResult(const std::shared_ptr<AdTaskHandle>&, size_t)> continuation;
    uint64_t checkpoints{0};
};

struct AdScheduledTask {
    std::shared_ptr<AdTaskHandle> handle;
    std::shared_ptr<std::promise<Ad_Object*>> promise;
    std::function<AdRunSliceResult(const std::shared_ptr<AdTaskHandle>&, size_t)> run_slice;
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

    static bool in_worker_thread();

 private:
    static void* worker_entry(void* self);
    void workerLoop();
    void finishTask(const std::shared_ptr<AdScheduledTask>& task, Ad_Object* value, AdTaskStatus status);
    void failTask(const std::shared_ptr<AdScheduledTask>& task, AdTaskStatus status);
    void enqueueTask(std::shared_ptr<AdScheduledTask> task);

    size_t num_workers_{0};
    std::vector<pthread_t> workers_;
    std::mutex mu_;
    std::condition_variable cv_;
    std::queue<std::shared_ptr<AdScheduledTask>> queue_;
    bool stop_{false};
    std::atomic<size_t> quantum_budget_{20000};

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

void ad_tls_set_task_ctx(AdTaskExecutionContext* ctx);
void ad_tls_clear_task_ctx(AdTaskExecutionContext* prev);
AdTaskExecutionContext* ad_tls_task_ctx();
void ad_task_checkpoint();

Ad_Object* ad_task_join_handle(const std::shared_ptr<AdTaskHandle>& h, GarbageCollector* gc);

void ad_set_global_task_scheduler(std::shared_ptr<TaskScheduler> s);
std::shared_ptr<TaskScheduler> ad_global_task_scheduler();
