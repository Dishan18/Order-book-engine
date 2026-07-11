#include <array>
#include <iostream>
#include <cstring>
#include <thread>
#include <atomic>
#include <csignal>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <emmintrin.h> // Required for _mm_pause()
#include "network/tcp_server.hpp"
#include "protocol/parser.hpp"
#include "engine/order_book.hpp"
#include "common/lock_free_queue.hpp" // NEW INCLUDE

std::atomic<bool> server_running{true};

void handle_signal(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n[SYSTEM] SIGINT received. Initiating graceful shutdown...\n";
        server_running.store(false, std::memory_order_relaxed);
    }
}

int main()
{
    std::signal(SIGINT, handle_signal);

    try {
        mfe::TcpServer server(8080);
        server.start();

        mfe::OrderBook book;
        
        // Lock-Free Queue initialized with a capacity of 1,048,576 elements
        mfe::LockFreeQueue<mfe::Message, 8192> message_queue;

        // ==========================================
        // CONSUMER THREAD: The Order Book Engine
        // ==========================================
        std::thread engine_thread([&book, &message_queue]() {
            std::cout << "[ENGINE] Thread started. Awaiting messages...\n";
            
            std::vector<std::uint64_t> latencies;
            latencies.reserve(20000000); // Pre-allocate room for 20 million pings
            int empty_spin_count = 0;
            while (true) {
                mfe::Message parsed_msg;
                
                // Spin-wait: rapidly poll the queue
                if (message_queue.pop(parsed_msg)) {
                    empty_spin_count = 0;
                    std::uint64_t creation_timestamp = 0;

                    std::visit([&book, &creation_timestamp](const auto& msg) {
                        using T = std::decay_t<decltype(msg)>;
                        creation_timestamp = msg.timestamp;
                        
                        if constexpr (std::is_same_v<T, mfe::AddOrderMessage>) {
                            book.add(mfe::Order{
                                .order_id = msg.order_id, .price = msg.price, .quantity = msg.quantity,
                                .side = msg.side, .timestamp = msg.timestamp, .symbol = msg.symbol
                            });
                        }
                        else if constexpr (std::is_same_v<T, mfe::CancelOrderMessage>) {
                            book.cancel(msg.order_id);
                        }
                        else if constexpr (std::is_same_v<T, mfe::ModifyOrderMessage>) {
                            book.modify(msg.order_id, msg.new_quantity, msg.new_price, msg.timestamp);
                        }
                        else if constexpr (std::is_same_v<T, mfe::TradeMessage>) {
                            book.cancel(msg.order_id);
                        }
                    }, parsed_msg);

                    auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   std::chrono::steady_clock::now().time_since_epoch()).count();
                    
                    latencies.push_back(now - creation_timestamp);
                } else {
                    // Queue is currently empty. Are we shutting down?
                    if (!server_running.load(std::memory_order_relaxed)) {
                        std::cout << "[ENGINE] Queue drained and stopped. Calculating benchmarks...\n";
                        
                        if (!latencies.empty()) {
                            std::sort(latencies.begin(), latencies.end());
                            
                            double average = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
                            std::uint64_t p50 = latencies[latencies.size() * 0.50];
                            std::uint64_t p90 = latencies[latencies.size() * 0.90];
                            std::uint64_t p99 = latencies[latencies.size() * 0.99];
                            std::uint64_t max = latencies.back();

                            std::cout << "\n=========================================\n";
                            std::cout << "       LOCK-FREE BENCHMARK REPORT        \n";
                            std::cout << "=========================================\n";
                            std::cout << "Total Messages Processed : " << latencies.size() << "\n";
                            std::cout << "Average Latency          : " << std::fixed << std::setprecision(0) << average << " ns\n";
                            std::cout << "Median (p50)             : " << p50 << " ns\n";
                            std::cout << "90th Percentile (p90)    : " << p90 << " ns\n";
                            std::cout << "99th Percentile (p99)    : " << p99 << " ns\n";
                            std::cout << "Max Latency              : " << max << " ns\n";
                            std::cout << "=========================================\n";
                        }
                        break; 
                    }
                    if (empty_spin_count < 100) {
                        _mm_pause(); // x86 PAUSE: Stays on CPU, prevents pipeline flush
                        empty_spin_count++;
                    } else {
                        std::this_thread::yield(); // Surrender to OS scheduler
                    }
                }
            }
        });

        // ==========================================
        // PRODUCER THREAD: The Network I/O Loop
        // ==========================================
        std::array<std::uint8_t, 2> length_buffer;
        std::array<std::uint8_t, 256> payload_buffer;

        std::cout << "[NETWORK] Listening for stream...\n";

        while (server_running.load(std::memory_order_relaxed))
        {
            if (!server.read_exact(length_buffer)) break;

            std::uint16_t payload_len = 0;
            std::memcpy(&payload_len, length_buffer.data(), sizeof(payload_len));

            if (payload_len > payload_buffer.size()) break;

            std::span<std::uint8_t> actual_msg_span(payload_buffer.data(), payload_len);
            if (!server.read_exact(actual_msg_span)) break;

            mfe::Message parsed_msg = mfe::Parser::parse(actual_msg_span);
            
            // Push into lock-free queue. If full, spin until space is available.
            int full_spin_count = 0;
            
            while (!message_queue.push(parsed_msg)) { // Notice we removed std::move here to allow retry if push fails
                if (!server_running.load(std::memory_order_relaxed)) break;
                
                if (full_spin_count < 100) {
                    _mm_pause();
                    full_spin_count++;
                } else {
                    std::this_thread::yield();
                }
            }
        }

        std::cout << "[SYSTEM] Network loop terminated. Stopping message queue...\n";
        server_running.store(false, std::memory_order_relaxed);
        
        if (engine_thread.joinable()) {
            engine_thread.join();
        }

        std::cout << "[SYSTEM] Shutdown complete. Final Book Size: " << book.active_order_count() << "\n";
    } 
    catch (const std::exception& e) {
        std::cerr << "[SYSTEM] Fatal Error: " << e.what() << "\n";
    }

    return 0;
}