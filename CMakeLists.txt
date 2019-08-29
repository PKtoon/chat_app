cmake_minimum_required(VERSION 2.8)

project(chat_app)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lboost_serialization")

file(GLOB SERVER_SOURCES "server/*.h" "server/*.cpp")
file(GLOB CLIENT_SOURCES "client/*.h" "client/*.cpp")
#file(GLOB STREAM_SOURCES "stream/stream.*")

file( GLOB LIB "stream/*" )
add_library( "stream" ${LIB} )

add_executable("server"  ${SERVER_SOURCES})
add_executable("client" ${CLIENT_SOURCES})

target_link_libraries( "server" "stream" )
target_link_libraries( "client" "stream" )
#add_executable("demo" demo.cpp)
