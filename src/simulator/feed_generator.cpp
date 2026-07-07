#include "simulator/feed_generator.hpp"
#include <array>
#include <chrono>
#include <algorithm>
namespace mfe {
FeedGenerator::FeedGenerator()
    : rng_(std::random_device{}()),
      price_dist_(10000, 20000),
      quantity_dist_(1, 1000),
      side_dist_(0, 1),
      next_order_id_(1000)
{
}
AddOrderMessage FeedGenerator::generate_add_order()
{
    AddOrderMessage message;
    message.order_id = next_order_id_++;
    message.timestamp =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
    static constexpr std::array<Symbol, 5> symbols = {{
        {'A','A','P','L'},
        {'M','S','F','T'},
        {'N','V','D','A'},
        {'A','M','Z','N'},
        {'G','O','O','G'}
    }};
    std::uniform_int_distribution<std::size_t> symbol_dist(
        0,
        symbols.size() - 1);
    message.symbol = symbols[symbol_dist(rng_)];
    message.price = price_dist_(rng_);
    message.quantity = quantity_dist_(rng_);
    message.side =
        side_dist_(rng_) == 0
            ? Side::Buy
            : Side::Sell;

    active_orders_[message.order_id] = message;
    active_order_ids_.push_back(message.order_id);    
    return message;
}
ModifyOrderMessage FeedGenerator::generate_modify_order()
{
    ModifyOrderMessage message{};
    if (active_order_ids_.empty())
        return message;
    std::uniform_int_distribution<std::size_t> order_dist(
        0,
        active_order_ids_.size() - 1);
    auto order_id = active_order_ids_[order_dist(rng_)];
    auto& order = active_orders_.at(order_id);
    message.order_id = order.order_id;
    message.timestamp =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
    message.new_price = price_dist_(rng_);
    message.new_quantity = quantity_dist_(rng_);
    order.price = message.new_price;
    order.quantity = message.new_quantity;
    return message;
}
CancelOrderMessage FeedGenerator::generate_cancel_order()
{
    CancelOrderMessage message{};
    if (active_order_ids_.empty())
        return message;
    std::uniform_int_distribution<std::size_t> order_dist(
        0,
        active_order_ids_.size() - 1);
    std::size_t index = order_dist(rng_);
    auto order_id = active_order_ids_[index];
    message.order_id = order_id;
    message.timestamp =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
    active_orders_.erase(order_id);
    active_order_ids_[index] = active_order_ids_.back();
    active_order_ids_.pop_back();
    return message;
}
TradeMessage FeedGenerator::generate_trade()
{
    TradeMessage message{};
    if (active_order_ids_.empty())
        return message;
    std::uniform_int_distribution<std::size_t> order_dist(
        0,
        active_order_ids_.size() - 1);
    std::size_t index = order_dist(rng_);
    auto order_id = active_order_ids_[index];
    auto& order = active_orders_.at(order_id);
    message.order_id = order.order_id;
    message.timestamp =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
    std::uniform_int_distribution<std::uint32_t> trade_dist(
        1,
        order.quantity);
    message.traded_quantity = trade_dist(rng_);
    order.quantity -= message.traded_quantity;
    if (order.quantity == 0)
    {
        active_orders_.erase(order_id);
        active_order_ids_[index] = active_order_ids_.back();
        active_order_ids_.pop_back();
    }
    return message;
}
std::size_t FeedGenerator::active_order_count() const
{
    return active_orders_.size();
}
}