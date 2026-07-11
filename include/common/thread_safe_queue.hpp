#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace mfe {

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    // Delete copy and move semantics to prevent accidental copying of mutexes
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;
    ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

    // Pushes an element and wakes up the sleeping consumer thread
    void push(T value) 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cond_var_.notify_one();
    }

    // Consumer waits here until data arrives or the system is shutting down
    std::optional<T> wait_and_pop() 
    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Wait releases the mutex and sleeps. It wakes up when notified AND the lambda condition is true.
        cond_var_.wait(lock, [this] { 
            return !queue_.empty() || stop_; 
        });
        
        // If the system is shutting down and all messages are processed, exit gracefully
        if (stop_ && queue_.empty()) {
            return std::nullopt;
        }

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    // Signals the consumer thread to wake up and exit
    void stop() 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        cond_var_.notify_all();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
    bool stop_{false};
};

}