#include "protocol/serializer.hpp"
#include "protocol/binary_writer.hpp"

namespace mfe {
std::vector<std::uint8_t>
Serializer::serialize(const AddOrderMessage& message)
{
    std::vector<std::uint8_t> buffer;
    BinaryWriter writer(buffer);
    writer.write(MessageType::Add);
    writer.write(message.order_id);
    writer.write(message.timestamp);
    writer.write_bytes(message.symbol.data(), message.symbol.size());
    writer.write(message.price);
    writer.write(message.quantity);
    writer.write(message.side);
    return buffer;
}
std::vector<std::uint8_t>
Serializer::serialize(const ModifyOrderMessage& message)
{
    std::vector<std::uint8_t> buffer;
    BinaryWriter writer(buffer);
    writer.write(MessageType::Modify);
    writer.write(message.order_id);
    writer.write(message.timestamp);
    writer.write(message.new_quantity);
    writer.write(message.new_price);
    return buffer;
}
std::vector<std::uint8_t>
Serializer::serialize(const CancelOrderMessage& message)
{
    std::vector<std::uint8_t> buffer;
    BinaryWriter writer(buffer);
    writer.write(MessageType::Cancel);
    writer.write(message.order_id);
    writer.write(message.timestamp);
    return buffer;
}
std::vector<std::uint8_t>
Serializer::serialize(const TradeMessage& message)
{
    std::vector<std::uint8_t> buffer;
    BinaryWriter writer(buffer);
    writer.write(MessageType::Trade);
    writer.write(message.order_id);
    writer.write(message.timestamp);
    writer.write(message.traded_quantity);
    return buffer;
}
}