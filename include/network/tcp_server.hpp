#pragma once

#include <cstdint>
#include <cstddef>
#include <span>

namespace mfe {

class TcpServer {
public:
    explicit TcpServer(std::uint16_t port);
    ~TcpServer();
    void start();
    std::size_t receive(std::span<std::uint8_t> buffer);
    // Reads EXACTLY size bytes into the buffer. Blocks until complete or connection drops.
    bool read_exact(std::span<std::uint8_t> buffer);

private:
    std::uint16_t port_;
    int server_fd_{-1}; 
    int client_fd_{-1}; 
};

}