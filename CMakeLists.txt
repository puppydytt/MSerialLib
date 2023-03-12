cmake_minimum_required(VERSION 3.21.2)

project(TestForSerialLib)

if (NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "C compiler not found")
endif ()

if (NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "C++ compiler not found")
endif ()

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(MSerialLib)

add_executable(SerialMonitor UsageExampleSerialMonitor.cpp)
target_link_libraries(SerialMonitor MSerialLib)