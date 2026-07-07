#pragma once
#include <cstdint>
#include "protocol/message.hpp"

namespace mfe {
struct Order {
    std::uint64_t order_id{};
    Price price{};
    std::uint32_t quantity{};
    Side side{};
    std::uint64_t timestamp{};
    Symbol symbol{};
};
}