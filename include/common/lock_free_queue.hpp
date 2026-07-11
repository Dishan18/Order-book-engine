#pragma once

#include <atomic>
#include <vector>
#include <cstddef>
#include <stdexcept>

namespace mfe {

// Capacity MUST be a power of 2 so we can use ultra-fast bitwise masking (&) instead of modulo (%)
template<typename T, std::size_t Capacity = 1048576> 
class LockFreeQueue {
    static_assert((Capacity != 0) && ((Capacity & (Capacity - 1)) == 0), "Capacity must be a power of 2");

public:
    LockFreeQueue() : buffer_(Capacity) {}

    // Producer only: returns true if pushed, false if queue is full
    bool push(T value) {
        auto current_head = head_.load(std::memory_order_relaxed);
        // Acquire memory order ensures we see the consumer's most recent tail update
        auto current_tail = tail_.load(std::memory_order_acquire); 

        // If the queue is full, we must reject the push
        if (current_head - current_tail == Capacity) {
            return false; 
        }

        // Bitwise mask maps the infinitely growing head index directly into our buffer bounds
        buffer_[current_head & (Capacity - 1)] = std::move(value);
        
        // Release memory order ensures the data is fully written to memory BEFORE head is incremented
        head_.store(current_head + 1, std::memory_order_release);
        return true;
    }

    // Consumer only: returns true if popped, false if queue is empty
    bool pop(T& out_value) {
        auto current_tail = tail_.load(std::memory_order_relaxed);
        // Acquire memory order ensures we see the producer's most recent head update
        auto current_head = head_.load(std::memory_order_acquire); 

        // If head == tail, the queue is completely empty
        if (current_tail == current_head) {
            return false; 
        }

        out_value = std::move(buffer_[current_tail & (Capacity - 1)]);
        
        // Release memory order ensures we extract the data BEFORE the tail is incremented
        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }

private:
    // HARDWARE OPTIMIZATION: 
    // alignas(64) physically forces these variables onto separate CPU cache lines 
    // to completely eliminate "False Sharing" contention between the two threads. prevent cache line ping-pong
    alignas(64) std::atomic<std::size_t> head_{0}; 
    alignas(64) std::atomic<std::size_t> tail_{0}; 
    alignas(64) std::vector<T> buffer_;
};

}