#pragma once

#include <random>
#include <variant>

#include "protocol/message.hpp"
#include "simulator/feed_generator.hpp"

namespace mfe {

using MarketEvent = std::variant<
    AddOrderMessage,
    ModifyOrderMessage,
    CancelOrderMessage,
    TradeMessage>;

class MessageDispatcher
{
public:
    MessageDispatcher();
    MarketEvent next();
private:
    FeedGenerator generator_;
    std::mt19937 rng_;
    std::discrete_distribution<int> event_dist_;
};

}