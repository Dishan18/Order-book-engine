#include <array>
#include <iostream>
#include <cstring>
#include "network/tcp_server.hpp"
#include "protocol/parser.hpp"
#include "engine/order_book.hpp"

int main()
{
    try {
        mfe::TcpServer server(8080);
        server.start();
        mfe::OrderBook book;

        std::array<std::uint8_t, 2> length_buffer;
        std::array<std::uint8_t, 256> payload_buffer;

        std::cout << "Awaiting streamed market events...\n";

        while (true)
        {
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

            std::span<std::uint8_t> actual_msg_span(payload_buffer.data(), payload_len);
            if (!server.read_exact(actual_msg_span)) {
                std::cout << "Client disconnected prematurely.\n";
                break;
            }

            mfe::Message parsed_msg = mfe::Parser::parse(actual_msg_span);
            std::visit([&book](const auto& msg) {
                using T = std::decay_t<decltype(msg)>;
                
                if constexpr (std::is_same_v<T, mfe::AddOrderMessage>) {
                    book.add(mfe::Order{
                        .order_id = msg.order_id,
                        .price = msg.price,
                        .quantity = msg.quantity,
                        .side = msg.side,
                        .timestamp = msg.timestamp,
                        .symbol = msg.symbol
                    });
                    std::cout << "[ADD] ID: " << msg.order_id 
                              << " | Active Book Size: " << book.active_order_count() << "\n";
                }
                else if constexpr (std::is_same_v<T, mfe::CancelOrderMessage>) {
                    book.cancel(msg.order_id);
                    std::cout << "[CANCEL] ID: " << msg.order_id 
                              << " | Active Book Size: " << book.active_order_count() << "\n";
                }
                else if constexpr (std::is_same_v<T, mfe::ModifyOrderMessage>) {
                    book.modify(msg.order_id, msg.new_quantity, msg.new_price, msg.timestamp);
                    std::cout << "[MODIFY] ID: " << msg.order_id 
                              << " | Active Book Size: " << book.active_order_count() << "\n";
                }
                else if constexpr (std::is_same_v<T, mfe::TradeMessage>) {
                    book.cancel(msg.order_id);
                    std::cout << "[TRADE EXECUTION] ID: " << msg.order_id 
                              << " | Active Book Size: " << book.active_order_count() << "\n";
                }
            }, parsed_msg);
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Server Error: " << e.what() << "\n";
    }

    return 0;
}