set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
include_directories(${PROJECT_SOURCE_DIR}/ThreadAndNetWorkCoding)
add_executable(test_base  test_base.cpp)
target_link_libraries(test_base zdq_utils)

add_executable(test_thread_pool  test_thread_pool.cpp)
target_link_libraries(test_thread_pool zdq_utils)

add_executable(test_buffer test_buffer.cpp)
target_link_libraries(test_buffer zdq_utils)

add_executable(test_event_loop test_event_loop.cpp)
target_link_libraries(test_event_loop zdq_utils)

add_executable(test_time_stamp test_time_stamp.cpp)
target_link_libraries(test_time_stamp zdq_utils)

add_executable(test_eventloop_poller_channel test_eventloop_poller_channel.cpp)
target_link_libraries(test_eventloop_poller_channel zdq_utils)

add_executable(test_timer test_timer.cpp)
target_link_libraries(test_timer zdq_utils)

add_executable(test_atomic test_atomic.cpp)
target_link_libraries(test_atomic zdq_utils)

add_executable(test_timer_queue test_timer_queue.cpp)
target_link_libraries(test_timer_queue zdq_utils)

add_executable(test_eventloop_thread test_eventloop_thread.cpp)
target_link_libraries(test_eventloop_thread zdq_utils)

add_executable(test_acceptor_server test_acceptor_server.cpp)
target_link_libraries(test_acceptor_server zdq_utils)

add_executable(test_acceptor_client test_acceptor_client.cpp)
target_link_libraries(test_acceptor_client zdq_utils)

add_executable(test_TcpServerAndTcpConnection_server test_TcpServerAndTcpConnection_server.cpp)
target_link_libraries(test_TcpServerAndTcpConnection_server zdq_utils)

add_executable(test_eventloop_threadpool test_eventloop_threadpool.cpp)
target_link_libraries(test_eventloop_threadpool zdq_utils)

add_executable(test_Connector test_Connector.cpp)
target_link_libraries(test_Connector zdq_utils)

