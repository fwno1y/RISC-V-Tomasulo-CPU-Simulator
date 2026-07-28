cmake_minimum_required(VERSION 3.29)
project(Tomasulo_CPU)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(code
        src/main.cpp
        src/Reg.cpp
        src/RS.cpp
        src/ROB.cpp
        src/Instruction.cpp
        src/Memory.cpp
        src/LSQ.cpp
        src/ALU.cpp
        src/Predictor.cpp
        src/CDB.cpp)