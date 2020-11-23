cmake_minimum_required(VERSION 2.8)

project(chat_app)

set(CMAKE_CXX_STANDARD 17)

set(LIB "-pthread")
    
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32_LEAN_AND_MEAN")
    set(LIB "${LIB}" "-lws2_32")
endif(WIN32)

include_directories(include/ external/asio/asio/include/ external/json/single_include/)

add_subdirectory(src/network-interface)
add_subdirectory(src/server)
add_subdirectory(src/client)

enable_testing()
add_subdirectory(tests/)
