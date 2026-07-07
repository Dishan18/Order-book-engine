#include "protocol/parser.hpp"
#include "protocol/binary_reader.hpp"
#include <stdexcept>
namespace mfe {
Message Parser::parse(const std::vector<std::uint8_t>& buffer)
{
    BinaryReader reader(buffer);
    auto type = reader.read<MessageType>();
    switch (type)
    {
        case MessageType::Add:
        {
            AddOrderMessage msg;
            msg.order_id = reader.read<std::uint64_t>();
            msg.timestamp = reader.read<std::uint64_t>();
            reader.read_bytes(
                msg.symbol.data(),
                msg.symbol.size());
            msg.price = reader.read<double>();
            msg.quantity = reader.read<std::uint32_t>();
            msg.side = reader.read<Side>();
            return msg;
        }
        case MessageType::Modify:
        {
            ModifyOrderMessage msg;
            msg.order_id = reader.read<std::uint64_t>();
            msg.timestamp = reader.read<std::uint64_t>();
            msg.new_quantity = reader.read<std::uint32_t>();
            msg.new_price = reader.read<double>();
            return msg;
        }

        case MessageType::Cancel:
        {
            CancelOrderMessage msg;
            msg.order_id = reader.read<std::uint64_t>();
            msg.timestamp = reader.read<std::uint64_t>();
            return msg;
        }
        case MessageType::Trade:
        {
            TradeMessage msg;
            msg.order_id = reader.read<std::uint64_t>();
            msg.timestamp = reader.read<std::uint64_t>();
            msg.traded_quantity = reader.read<std::uint32_t>();
            return msg;
        }
        default:
            throw std::runtime_error("Unknown message type");
    }
}

}