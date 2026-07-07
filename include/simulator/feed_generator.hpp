#pragma once
#include <random>
#include <unordered_map>
#include <vector>
#include "protocol/message.hpp"
namespace mfe {
class FeedGenerator
{
public:
    FeedGenerator();

    AddOrderMessage generate_add_order();
    ModifyOrderMessage generate_modify_order();
    CancelOrderMessage generate_cancel_order();
    TradeMessage generate_trade();
    std::size_t active_order_count() const;
private:
    std::unordered_map<std::uint64_t, AddOrderMessage> active_orders_;
    std::vector<std::uint64_t> active_order_ids_;
    std::mt19937_64 rng_;
    std::uniform_int_distribution<std::int64_t> price_dist_;
    std::uniform_int_distribution<std::uint32_t> quantity_dist_;
    std::uniform_int_distribution<int> side_dist_;
    std::uint64_t next_order_id_;
};
}