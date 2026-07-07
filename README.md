# Low-Latency Market Data Feed Handler & Order Book

A production-grade, C++20 quantitative trading engine simulator. This project models the full pipeline of a financial exchange, featuring a zero-allocation binary network protocol, deterministic TCP stream framing, and a highly optimized Limit Order Book (LOB).

## Core Architecture

- **Exchange Simulator (Producer):** Generates synthetic market events (Add, Modify, Cancel, Trade) and streams them over a custom binary protocol.
- **Trading Engine (Consumer):** Parses the incoming byte stream and maintains a real-time representation of the market.
- **Zero-Allocation Hot Path:** Employs `std::span` and stack-allocated arrays to completely eliminate dynamic heap allocations (`malloc`/`new`) during network serialization and parsing.
- **Deterministic Framing:** Overcomes TCP stream fragmentation using a strict length-prefixed binary framing protocol.
- **High-Performance Order Book:** - $O(1)$ Average Order Cancellation & Modification via `std::unordered_map` caching doubly-linked list iterators (`std::list`).
  - $O(\log P)$ Price Level indexing via Red-Black Trees (`std::map`).
  - Strict Price-Time Priority (FIFO) enforcement.

## Tech Stack
- **Language:** C++20 (utilizing concepts, `std::span`, and variant routing)
- **Networking:** Linux Sockets (TCP/IPv4, `TCP_NODELAY` enabled)
- **Build System:** CMake
- **Environment:** WSL / Linux

## Build Instructions

```bash
mkdir -p build && cd build
cmake ..
make

Running the Engine
This system requires two separate processes to simulate the network boundary.

1. Start the Trading Engine (Server)

Bash
./feed_server

2. Start the Exchange Simulator (Client)

Bash
./feed_client