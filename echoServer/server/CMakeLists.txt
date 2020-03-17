#aux_source_directory(. SRCS)
#add_executable(server ${SRCS})

#add_executable(server selectServer.cpp)

#add_executable(server libeventServer.cpp)
#target_link_libraries(server event)

#add_executable(server epollServer.cpp)

add_executable(server libevent/advanced.cpp)
target_link_libraries(server event pthread event_pthreads)

