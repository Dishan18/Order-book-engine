#pragma once
#include <vector>
#include "protocol/message.hpp"
namespace mfe {
class Serializer
{
public:
    static std::vector<std::uint8_t>
    serialize(const AddOrderMessage&);

    static std::vector<std::uint8_t>
    serialize(const ModifyOrderMessage&);

    static std::vector<std::uint8_t>
    serialize(const CancelOrderMessage&);

    static std::vector<std::uint8_t>
    serialize(const TradeMessage&);
};
}