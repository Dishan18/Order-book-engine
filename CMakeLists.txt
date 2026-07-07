cmake_minimum_required(VERSION 3.22)

project(
    MarketFeedEngine
    VERSION 1.0
    DESCRIPTION "Low-Latency Market Data Feed Handler & Order Book"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
    market_feed_engine
    src/main.cpp
    src/protocol/serializer.cpp
    src/protocol/parser.cpp
    src/simulator/feed_generator.cpp
    src/simulator/message_dispatcher.cpp
)

target_include_directories(
    market_feed_engine
    PRIVATE
    include
)