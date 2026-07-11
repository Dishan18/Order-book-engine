# Project State Context: Market Feed Engine

## Current System State (Phase 1-5 Completed)
- **Networking:** TCP client/server configured with `TCP_NODELAY`. Framing handles stream fragmentation.
- **Protocol:** Zero-allocation binary serialization/parsing using `std::span`.
- **Order Book Engine:** Dual-structure matching engine (`std::map` and `std::unordered_map` index).
- **Concurrency:** Lock-Free SPSC queue using `std::atomic`, cache-line padding, and adaptive backoff (`_mm_pause()`).
- **Baseline Latency:** Wire-to-Engine p50 is ~7 µs in a virtualized WSL environment.

## The Engineering Roadmap (Measure → Identify → Optimize)

### Phase 6: Performance Profiling & Memory Audit (Current Focus)
- [ ] **Memory Audit:** Compile with `-fsanitize=address` to verify zero leaks, invalid reads, or buffer overruns.
- [ ] **CPU Profiling (`perf`):** Measure instructions per cycle (IPC), cache misses, and branch misses.
- [ ] **Hotspot Analysis:** Generate a Flame Graph via `perf record` to identify the exact functions burning CPU cycles.

### Phase 7: Architecture Improvements (Data-Driven)
- Evaluate replacing `std::map` with a flat sorted vector, B-tree, or Radix tree to reduce pointer-chasing and cache misses.
- Evaluate network batching (reading multiple messages per `recv()`) to optimize throughput.

### Phase 8 & 9: Native Benchmarking & Documentation
- Deploy on a native Linux bare-metal machine.
- Implement thread affinity (`pthread_setaffinity_np` / `isolcpus`).
- Publish a comprehensive Performance Report (Architecture, Memory Layout, Flame Graphs, Before/After p50/p99 metrics).