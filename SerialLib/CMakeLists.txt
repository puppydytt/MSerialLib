cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)

set(src
        driver.cpp
        ReceiveData.cpp
        SerialWrite.cpp
        listenCommPort.cpp
        )

add_library(MySerial STATIC ${src})

target_include_directories(MySerial PUBLIC serialLib.h)

