#add_executable(simpleClient.cpp)

add_executable(client libevent/primary.cpp)
target_link_libraries(client event)
