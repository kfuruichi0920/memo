cmake_minimum_required(VERSION 3.13)
project(cmake_sock_test C)

#set(CMAKE_C_FLAGS "-std=c11 -Wall -pedantic")

# cmake -DCMAKE_BUILD_TYPE=Debug ..
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -pg")
# cmake -DCMAKE_BUILD_TYPE=Release ..
set(CMAKE_C_FLAGS_RELEASE "-O2 -s")

add_executable(sock_test 
	sock_test.c 
	sock_tcp.c 
	sock_udp.c 
	get_ifinfo.c)
