cmake_minimum_required(VERSION 2.8)

project(chat_app)

#SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I /home/pratik/usr/pk/include/ -L /home/pratik/usr/pk/lib/ -pthread")
#LINK_DIRECTORIES("/home/pratik/usr/pk/lib")

#add_executable(${PROJECT_NAME} "server.cpp")

add_executable("server" "server.cpp")
add_executable("client" "client.cpp")
#add_executable("demo" "demo.cpp")
