# Project State Context: Market Feed Engine

## Current System State (Phase 3 & 4 Completed)
- **Networking:** Fully operational. TCP client/server configured with `TCP_NODELAY` and `SO_REUSEADDR`. Network framing (2-byte length prefix) is successfully handling TCP stream fragmentation.
- **Protocol:** Zero-allocation binary serialization/parsing using C++20 `std::span` and type concepts. Fixed-point arithmetic (`int64_t`) implemented for price data to meet quant standards.
- **Order Book Engine:** `OrderBook` class implemented utilizing a dual-structure approach:
  - `std::map<Price, std::list<Order>>` for sorted price levels (Bids descending, Asks ascending).
  - `std::unordered_map<std::uint64_t, OrderIndexEntry>` caching `std::list::iterator`, `Price`, and `Side` for $O(1)$ operations.
- **Integration:** The `feed_server` successfully reads the continuous byte stream, parses it via `std::visit`, and routes it to the `OrderBook` to process `add`, `modify`, `cancel`, and `trade` events in real-time.

## Pending Work (Concurrency, Benchmarking & Optimization)
The bottleneck is currently the synchronous single-thread execution. We are moving to a Producer-Consumer architecture using an incremental, verifiable approach.

### Phase 4: Baseline Concurrency
- [ ] Implement `ThreadSafeQueue` (`std::mutex` + `std::condition_variable`).
- [ ] Split architecture into Producer Thread (Network I/O) and Consumer Thread (Order Book Engine).
- [ ] Implement graceful shutdown sequence.
- [ ] Run baseline performance benchmarks.

### Phase 5: Lock-Free Implementation
- [ ] Replace `ThreadSafeQueue` with a Lock-Free Ring Buffer (`std::atomic`).
- [ ] Implement cache-line padding and prevent false-sharing.
- [ ] Run comparison benchmarks against the Phase 4 baseline.

### Phase 6: Deep Profiling
- [ ] Measure p50, p95, p99 latency statistics and throughput metrics.
- [ ] Utilize `perf` and flamegraphs to identify CPU hotspots.
- [ ] Optimize memory access patterns and branch prediction.