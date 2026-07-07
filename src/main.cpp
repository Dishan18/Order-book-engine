#include <iostream>
#include <variant>

#include "simulator/message_dispatcher.hpp"

int main()
{
    mfe::MessageDispatcher dispatcher;

    int add = 0;
    int modify = 0;
    int cancel = 0;
    int trade = 0;

    constexpr int iterations = 100000;

    for (int i = 0; i < iterations; ++i)
    {
        auto event = dispatcher.next();

        std::visit(
            [&](auto&& msg)
            {
                using T = std::decay_t<decltype(msg)>;

                if constexpr (std::is_same_v<T, mfe::AddOrderMessage>)
                    ++add;
                else if constexpr (std::is_same_v<T, mfe::ModifyOrderMessage>)
                    ++modify;
                else if constexpr (std::is_same_v<T, mfe::CancelOrderMessage>)
                    ++cancel;
                else if constexpr (std::is_same_v<T, mfe::TradeMessage>)
                    ++trade;
            },
            event);
    }

    std::cout << "ADD     : " << add << '\n';
    std::cout << "MODIFY  : " << modify << '\n';
    std::cout << "CANCEL  : " << cancel << '\n';
    std::cout << "TRADE   : " << trade << '\n';
}