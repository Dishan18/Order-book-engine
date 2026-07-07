#include <array>
#include <iostream>
#include <cstring>
#include "network/tcp_server.hpp"
#include "protocol/parser.hpp"

int main()
{
    try {
        mfe::TcpServer server(8080);
        server.start();

        std::array<std::uint8_t, 2> length_buffer;
        std::array<std::uint8_t, 256> payload_buffer;

        std::cout << "Awaiting streamed market events...\n";

        while (true)
        {
            // 1. Read exactly 2 bytes for the header
            if (!server.read_exact(length_buffer)) {
                std::cout << "Client disconnected.\n";
                break;
            }

            std::uint16_t payload_len = 0;
            std::memcpy(&payload_len, length_buffer.data(), sizeof(payload_len));

            if (payload_len > payload_buffer.size()) {
                std::cerr << "Fatal error: Message size exceeds buffer.\n";
                break;
            }

            // 2. Read exactly the payload length
            std::span<std::uint8_t> actual_msg_span(payload_buffer.data(), payload_len);
            if (!server.read_exact(actual_msg_span)) {
                std::cout << "Client disconnected prematurely.\n";
                break;
            }

            // 3. Parse and verify
            mfe::Message parsed_msg = mfe::Parser::parse(actual_msg_span);

            std::visit([](const auto& msg) {
                std::cout << "Received Order ID: " << msg.order_id 
                          << " | Timestamp: " << msg.timestamp << "\n";
            }, parsed_msg);
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Server Error: " << e.what() << "\n";
    }

    return 0;
}