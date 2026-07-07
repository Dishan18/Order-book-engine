#pragma once
#include <array>
#include <cstdint>

namespace mfe {
using Symbol = std::array<char, 8>;
using Price = std::int64_t;

enum class MessageType : std::uint8_t {
    Add = 1,
    Modify = 2,
    Cancel = 3,
    Trade = 4
};

enum class Side : std::uint8_t {
    Buy = 0,
    Sell = 1
};

struct AddOrderMessage {
    std::uint64_t order_id{};
    std::uint64_t timestamp{};
    Symbol symbol{};
    Price price{};
    std::uint32_t quantity{};
    Side side{};
};

struct ModifyOrderMessage {
    std::uint64_t order_id{};
    std::uint64_t timestamp{};
    std::uint32_t new_quantity{};
    Price new_price{};
};

struct CancelOrderMessage {
    std::uint64_t order_id{};
    std::uint64_t timestamp{};
};

struct TradeMessage {
    std::uint64_t order_id{};
    std::uint64_t timestamp{};
    std::uint32_t traded_quantity{};
};
}