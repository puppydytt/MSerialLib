cmake_minimum_required(VERSION 3.22)
project(programmerInterface)

set(CMAKE_CXX_STANDARD 23)

add_executable(programmerInterface main.cpp SerialLib/driver.cpp SerialLib/ReceiveData.cpp SerialLib/SerialWrite.cpp)
