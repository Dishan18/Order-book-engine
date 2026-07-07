#include "network/tcp_server.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>

namespace mfe {

TcpServer::TcpServer(std::uint16_t port)
    : port_(port)
{
}

TcpServer::~TcpServer()
{
    if (client_fd_ != -1) close(client_fd_);
    if (server_fd_ != -1) close(server_fd_);
}

void TcpServer::start()
{
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // QUALITY OF LIFE FIX: Allow port reuse to prevent bind errors during rapid restarts
    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        throw std::runtime_error("Failed to bind");
    }
    std::cout << "Server bound to port " << port_ << '\n';
    if (listen(server_fd_, 5) < 0) {
        throw std::runtime_error("Failed to listen");
    }
    std::cout << "Server listening...\n";
    sockaddr_in client_address{};
    socklen_t client_length = sizeof(client_address);
    client_fd_ = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_address), &client_length);
    if (client_fd_ < 0) {
        throw std::runtime_error("Failed to accept client");
    }

    // Disable Nagle's Algorithm on the client connection
    int flag = 1;
    setsockopt(client_fd_, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    std::cout << "Client connected\n";
}

std::size_t TcpServer::receive(std::span<std::uint8_t> buffer)
{
    auto bytes_received = recv(client_fd_, buffer.data(), buffer.size(), 0);
    if (bytes_received < 0) {
        throw std::runtime_error("Failed to receive");
    } else if (bytes_received == 0) {
        return 0;
    }
    return bytes_received;
}
bool TcpServer::read_exact(std::span<std::uint8_t> buffer)
{
    std::size_t total_bytes_read = 0;
    while (total_bytes_read < buffer.size())
    {
        auto bytes_received = recv(client_fd_, 
                                   buffer.data() + total_bytes_read, 
                                   buffer.size() - total_bytes_read, 
                                   0);
        if (bytes_received < 0) {
            throw std::runtime_error("Failed to read exact bytes from socket");
        }
        else if (bytes_received == 0) {
            return false; // Client disconnected gracefully
        }
        total_bytes_read += bytes_received;
    }
    return true;
}
}