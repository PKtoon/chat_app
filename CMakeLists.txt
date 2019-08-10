cmake_minimum_required(VERSION 2.8)

project(chat_app)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

file(GLOB SOURCES "server/*.h" "server/*.cpp")

add_executable("server" ${SOURCES})
add_executable("client" "client/main.cpp")
