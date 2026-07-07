#include "engine/order_book.hpp"
#include <iostream>

namespace mfe {
void OrderBook::add(const Order& order)
{
    if (order_index_.contains(order.order_id)) {
        return; 
    }
    OrderList::iterator it;

    if (order.side == Side::Buy) {
        // std::map automatically creates the OrderList if the price level doesn't exist
        auto& list = bids_[order.price];
        list.push_back(order);
        it = std::prev(list.end()); // Get iterator to the newly added element at the tail
    } else {
        auto& list = asks_[order.price];
        list.push_back(order);
        it = std::prev(list.end());
    }
    // 3. Cache the exact memory location and routing data for O(1) cancellation later
    order_index_[order.order_id] = OrderIndexEntry{
        .it = it,
        .price = order.price,
        .side = order.side
    };
}
void OrderBook::cancel(std::uint64_t order_id)
{
    // 1. O(1) Hash Map Lookup
    auto index_it = order_index_.find(order_id);
    if (index_it == order_index_.end()) {
        return; // Order doesn't exist (might have already been fully traded or cancelled)
    }

    const auto& entry = index_it->second;
    remove_from_book(entry.side, entry.price, entry.it);
    order_index_.erase(index_it);
}

void OrderBook::remove_from_book(Side side, Price price, OrderList::iterator it)
{
    if (side == Side::Buy) {
        auto map_it = bids_.find(price);
        if (map_it != bids_.end()) {
            // O(1) linked-list node detachment
            map_it->second.erase(it);
            
            // CRITICAL: Memory and Tree maintenance
            // If this price level has no more volume, delete the node from the map.
            // This keeps the O(log P) search space as small as possible.
            if (map_it->second.empty()) {
                bids_.erase(map_it);
            }
        }
    } else {
        auto map_it = asks_.find(price);
        if (map_it != asks_.end()) {
            map_it->second.erase(it);
            if (map_it->second.empty()) {
                asks_.erase(map_it);
            }
        }
    }
}

void OrderBook::modify(std::uint64_t order_id, std::uint32_t new_quantity, Price new_price, std::uint64_t timestamp)
{
    // 1. O(1) Hash Map Lookup
    auto index_it = order_index_.find(order_id);
    if (index_it == order_index_.end()) {
        return; // Order might have been cancelled
    }
    // 2. Extract the routing data and the actual physical order node
    const auto& entry = index_it->second;
    auto order_it = entry.it; 

    // 3. Evaluate Priority Rules
    bool price_changed = (order_it->price != new_price);
    bool quantity_increased = (new_quantity > order_it->quantity);

    if (price_changed || quantity_increased) {
        // PRIORITY LOST: The order must go to the back of the queue.
        
        // Copy the old order and apply the new state
        Order updated_order = *order_it;
        updated_order.price = new_price;
        updated_order.quantity = new_quantity;
        updated_order.timestamp = timestamp;
        
        // Remove the old physical order in O(1) time
        remove_from_book(entry.side, entry.price, entry.it);
        order_index_.erase(index_it);
        
        // Re-insert as a fresh order (which places it at the tail of the list)
        add(updated_order);
    } else {
        // PRIORITY RETAINED: The price is identical and quantity shrunk.
        // Update the physical node directly in memory. The list pointers remain untouched.
        order_it->quantity = new_quantity;
        order_it->timestamp = timestamp;
    }
}

}