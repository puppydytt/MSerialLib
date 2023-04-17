cmake_minimum_required(VERSION 3.21.2)

set(CMAKE_CXX_STANDARD 23)

set(src
        MSerialLib.cpp
        ReceiveData.cpp
        SerialWrite.cpp
        listenCommPort.cpp
        NonOverlappedComm/MSerialReadSimple.cpp NonOverlappedComm/MSerialWriteSimple.cpp)

add_library(MSerialLib STATIC ${src})

target_include_directories(MSerialLib PUBLIC MSerialLib.h)

