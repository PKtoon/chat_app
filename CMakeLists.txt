cmake_minimum_required(VERSION 2.8)

project(chat_app)

set(CMAKE_CXX_STANDARD 17)

include_directories(include/ external/asio/asio/include/ external/json/single_include/)

add_subdirectory(src/network-interface)
add_subdirectory(src/server)
add_subdirectory(src/client)

#enable_testing()
#add_subdirectory(tests/)
