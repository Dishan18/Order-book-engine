#pragma once

#include <cstdint>
#include <list>
#include <map>
#include <unordered_map>
#include <functional>
#include "engine/order.hpp"

namespace mfe {

class OrderBook {
public:
    // All orders at a specific price level
    using OrderList = std::list<Order>;
    // Bids: Sorted descending (highest buyer is first in line)
    using BidBook = std::map<Price, OrderList, std::greater<Price>>;
    // Asks: Sorted ascending (lowest seller is first in line)
    using AskBook = std::map<Price, OrderList>;

    OrderBook() = default;

    // Core Engine API
    void add(const Order& order);
    void cancel(std::uint64_t order_id);
    void modify(std::uint64_t order_id, std::uint32_t new_quantity, Price new_price, std::uint64_t timestamp);
    // Getters for market data
    std::size_t active_order_count() const { return order_index_.size(); }

private:
    BidBook bids_;
    AskBook asks_;
    // The O(1) Cancellation
    struct OrderIndexEntry {
        OrderList::iterator it;
        Price price;
        Side side;
    };
    // Maps an Order ID directly to its physical location in the lists
    std::unordered_map<std::uint64_t, OrderIndexEntry> order_index_;
    void remove_from_book(Side side, Price price, OrderList::iterator it);
};
}