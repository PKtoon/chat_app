cmake_minimum_required(VERSION 2.8)

project(chat_app)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

file(GLOB SERVER_SOURCES "server/*.h" "server/*.cpp")
file(GLOB CLIENT_SOURCES "client/*.h" "client/*.cpp")

add_executable("server" ${SERVER_SOURCES})
add_executable("client" ${CLIENT_SOURCES})
