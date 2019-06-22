#在linux平台下使用CMake生成Makefile并编译的流程如下：
#   1.编写CMake配置文件CMakeLists.txt。
#   2.执行命令cmake PATH或者ccmake PATH生成Makefile.
#     ccmake 和 cmake 的区别在于前者提供了一个交互式的界面。
#     其中，PATH是CMakeLists.txt所在的目录。
#   3.使用 make 命令进行编译。

cmake_minimum_required(VERSION 2.8) 

#项目信息
project(libcox)

# Set lib version 
set(CLIB_VERSION 0.5)

# Set cflags 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -O0 -Wall")

# Use c and asm 
enable_language(C ASM)

# 添加子目录
add_subdirectory(lib)

# include directory
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

# Macro for add test target 
macro(add_test_target EXAMPLE_TARGET) 
    # 指定生成目标
    add_executable("${EXAMPLE_TARGET}" "${EXAMPLE_TARGET}.cpp")
    # 添加链接库 
    target_link_libraries("${EXAMPLE_TARGET}" libcox_static pthread dl glog) 
endmacro(add_test_target)

add_test_target(event_loop_test)
add_test_target(connector_test)
add_test_target(tcp_client_test)
add_test_target(tcp_server_test)
