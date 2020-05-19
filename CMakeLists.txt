cmake_minimum_required(VERSION 3.16)
project(_Symulka_NOWA)

set(CMAKE_CXX_STANDARD 14)

include_directories(source)

add_executable(_Symulka_NOWA
        source/generator.h
        source/generator.cpp
    source/channel.cpp
    source/channel.h
    source/logger.cpp
    source/logger.h
    source/main.cpp
    source/packet.cpp
    source/packet.h
    source/process.cpp
    source/process.h
    source/rx.cpp
    source/rx.h
    source/Simulation.cpp
    source/Simulation.h
    source/tx.cpp
    source/tx.h
    source/wireless_network.cpp
    source/wireless_network.h)

