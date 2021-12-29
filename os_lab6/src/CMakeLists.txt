cmake_minimum_required(VERSION 3.18.4)
project(6_8_Vlad)

set(CMAKE_CXX_STANDARD 17)
add_definitions( -Wall -Werror -Wextra)
add_executable(control control_node.cpp)
add_executable(calculation_node calculation_node.cpp)
target_link_libraries(control zmq)
target_link_libraries(calculation_node zmq)