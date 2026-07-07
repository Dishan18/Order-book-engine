#pragma once

#include <variant>
#include <span>
#include "protocol/message.hpp"

namespace mfe {
using Message = std::variant<
    AddOrderMessage,
    ModifyOrderMessage,
    CancelOrderMessage,
    TradeMessage>;
class Parser {
public:
    static Message parse(std::span<const std::uint8_t> buffer);
};
}