#include <array>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include "simulator/message_dispatcher.hpp"
#include "protocol/serializer.hpp"
#include "network/tcp_client.hpp"

int main()
{
    try {
        mfe::TcpClient client("127.0.0.1", 8080);
        client.connect();

        mfe::MessageDispatcher dispatcher;
        std::array<std::uint8_t, 256> send_buffer;

        std::cout << "Starting market data stream...\n";

        while (true)
        {
            auto event = dispatcher.next();

            std::span<std::uint8_t> payload_span(send_buffer.data() + 2, send_buffer.size() - 2);
            std::size_t payload_len = 0;

            std::visit([&](const auto& msg) {
                payload_len = mfe::Serializer::serialize(msg, payload_span);
            }, event);

            std::uint16_t network_len = static_cast<std::uint16_t>(payload_len);
            std::memcpy(send_buffer.data(), &network_len, sizeof(network_len));

            std::size_t total_wire_bytes = sizeof(network_len) + payload_len;
            
            client.send(std::span<const std::uint8_t>(send_buffer.data(), total_wire_bytes));

            // Throttle slightly so we don't crash your WSL terminal with stdout printing
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Client Error: " << e.what() << "\n";
    }

    return 0;
}