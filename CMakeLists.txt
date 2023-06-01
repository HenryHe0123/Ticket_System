cmake_minimum_required(VERSION 3.16)
project(Ticket_System)

set(CMAKE_CXX_STANDARD 17)

include_directories(B+Tree)
include_directories(STLite)
include_directories(src)

add_executable(code
        main.cpp
        B+Tree/BPT.h
        B+Tree/multi_BPT.h
        STLite/algorithm.h
        STLite/exceptions.hpp
        STLite/map.hpp
        STLite/priority_queue.hpp
        STLite/utility.hpp
        STLite/vector.hpp
        STLite/myString.h
        B+Tree/data.h
        src/simpleScanner.h
        src/userSystem.h
        src/trainSystem.h
        src/myStruct.h)
