#pragma once

#include <cstdint>
#include <span>

namespace mfe {

class TcpClient {
public:
    TcpClient(const char* ip, std::uint16_t port);
    ~TcpClient();
    void connect();
    void send(std::span<const std::uint8_t> data);

private:
    const char* ip_;
    std::uint16_t port_;
    int socket_fd_{-1}; 
};

}