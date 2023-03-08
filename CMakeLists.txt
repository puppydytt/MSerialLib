cmake_minimum_required(VERSION 3.22)
project(TestForSerialLib)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(SerialLib)

add_executable(wtf main.cpp)
target_link_libraries(wtf MySerial)