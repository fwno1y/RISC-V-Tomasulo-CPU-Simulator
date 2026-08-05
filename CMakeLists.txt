cmake_minimum_required(VERSION 3.10)
project(RISC-V-CPU)

# 启用 C++17（可根据实际需要调整）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件搜索路径
include_directories(include)

# 收集 src 目录下的所有 .cpp 文件
file(GLOB SOURCES "src/*.cpp")

# 生成可执行文件 main
add_executable(main ${SOURCES})

# 可选：启用调试信息（如需调试）
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")