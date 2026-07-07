#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include "protocol/message.hpp"

namespace mfe {
class Serializer {
public:
    static std::size_t serialize(const AddOrderMessage& message, std::span<std::uint8_t> out_buffer);
    static std::size_t serialize(const ModifyOrderMessage& message, std::span<std::uint8_t> out_buffer);
    static std::size_t serialize(const CancelOrderMessage& message, std::span<std::uint8_t> out_buffer);
    static std::size_t serialize(const TradeMessage& message, std::span<std::uint8_t> out_buffer);
};
}