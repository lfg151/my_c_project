cmake_minimum_required(VERSION 3.10)
project(sm3_core)

set(CMAKE_C_STANDARD 99)

# 添加编译选项（开启警告）
add_compile_options(-Wall -Wextra)

# 生成可执行文件
add_executable(test_sm3 
    sm3_pad.c 
    sm3_group.c 
    test_sm3.c
)

# 链接m库（部分系统需要）
target_link_libraries(test_sm3 m)