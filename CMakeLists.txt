cmake_minimum_required(VERSION 3.10)  # 最低支持的CMake版本
project(sm3_project)                  # 项目名，和文件夹名一致

# 指定编译器为MinGW的gcc和g++
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_C_STANDARD 11)              # 使用C11标准

# 生成可执行文件，名为sm3_test，关联main.c源码
add_executable(sm3_test main.c)