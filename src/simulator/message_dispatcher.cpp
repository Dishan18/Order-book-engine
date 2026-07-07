#include "simulator/message_dispatcher.hpp"

namespace mfe {

MessageDispatcher::MessageDispatcher()
    : rng_(std::random_device{}()),
      event_dist_({70,15,10,5})
{
}

MarketEvent MessageDispatcher::next()
{
    if (generator_.active_order_count() == 0)
        return generator_.generate_add_order();
    switch(event_dist_(rng_))
    {
        case 0:
            return generator_.generate_add_order();

        case 1:
            return generator_.generate_modify_order();

        case 2:
            return generator_.generate_cancel_order();

        case 3:
            return generator_.generate_trade();

        default:
            return generator_.generate_add_order();
    }
}

}