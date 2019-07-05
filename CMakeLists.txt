cmake_minimum_required(VERSION 2.8)

project(chat_app)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

add_executable("server" "server.cpp")
add_executable("client" "client.cpp")
