cmake_minimum_required(VERSION 2.8.4)
project(speedTest)

set(CMAKE_CXX_STANDARD 11)

include_directories("../include")

add_executable(server performance/server.cpp)
target_link_libraries(server pthread)

add_executable(client performance/client.cpp)
target_link_libraries(client pthread)
