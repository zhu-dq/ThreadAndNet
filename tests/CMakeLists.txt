set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
include_directories(${PROJECT_SOURCE_DIR}/ThreadAndNetWorkCoding)
add_executable(test_base  test_base.cpp)
target_link_libraries(test_base zdq_utils)

add_executable(test_thread_pool  test_thread_pool.cpp)
target_link_libraries(test_thread_pool zdq_utils)

add_executable(test_buffer test_buffer.cpp)
target_link_libraries(test_thread_pool zdq_utils)
