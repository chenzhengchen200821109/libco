# Set cflags 
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -O0 -std=c++11 -Wall -fPIC")

# Add source files 
set(SOURCE_FILES buffer.cpp
                 inner_pre.cpp
                 sockets.cpp
                 inetaddress.cpp
                 event_loop.cpp
                 connector.cpp
                 tcp_conn.cpp
                 listener.cpp
                 tcp_client.cpp
                 tcp_server.cpp 
                 coctx.cpp
                 coctx_swap.S
                 co_epoll.cpp
                 co_hook_sys_call.cpp
                 co_routine.cpp)
# Add static and shared library target 
add_library(libcox_static STATIC ${SOURCE_FILES}) 
#add_library(libcox_shared SHARED ${SOURCE_FILES}) "${DIR_LIB_SRCS}")
