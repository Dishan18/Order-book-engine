#include "network/tcp_client.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>

namespace mfe {

TcpClient::TcpClient(const char* ip, std::uint16_t port)
    : ip_(ip), port_(port)
{
}

TcpClient::~TcpClient()
{
    if (socket_fd_ != -1) {
        close(socket_fd_);
    }
}

void TcpClient::connect()
{
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // CRITICAL FIX: Disable Nagle's Algorithm for Low Latency
    int flag = 1;
    if (setsockopt(socket_fd_, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) < 0) {
        throw std::runtime_error("Failed to set TCP_NODELAY");
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_);

    inet_pton(AF_INET, ip_, &server_address.sin_addr);

    if (::connect(socket_fd_, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
        throw std::runtime_error("Failed to connect");
    }

    std::cout << "Connected to server\n";
}

void TcpClient::send(std::span<const std::uint8_t> data)
{
    auto bytes_sent = ::send(socket_fd_, data.data(), data.size(), 0);

    if (bytes_sent < 0) {
        throw std::runtime_error("Failed to send");
    }
}

}