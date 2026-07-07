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

# Group all the shared backend files together
set(COMMON_SOURCES
    src/protocol/serializer.cpp
    src/protocol/parser.cpp
    src/simulator/feed_generator.cpp
    src/simulator/message_dispatcher.cpp
    src/network/tcp_server.cpp
    src/network/tcp_client.cpp
)

# 1. Build the Server Executable
add_executable(feed_server src/server_main.cpp ${COMMON_SOURCES})
target_include_directories(feed_server PRIVATE include)

# 2. Build the Client Executable
add_executable(feed_client src/client_main.cpp ${COMMON_SOURCES})
target_include_directories(feed_client PRIVATE include)