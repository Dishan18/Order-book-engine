#pragma once
#include <variant>
#include <vector>
#include "protocol/message.hpp"
namespace mfe {
using Message = std::variant<
    AddOrderMessage,
    ModifyOrderMessage,
    CancelOrderMessage,
    TradeMessage>;
class Parser
{
public:
    static Message parse(
        const std::vector<std::uint8_t>& buffer);
};
}