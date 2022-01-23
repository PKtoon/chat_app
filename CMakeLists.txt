cmake_minimum_required(VERSION 2.8)

project(chat_app)

set(CMAKE_CXX_STANDARD 17)

include_directories(external/asio/asio/include/ external/json/single_include/)

add_subdirectory(src/network-interface)
add_subdirectory(external/sqlite3)
add_subdirectory(src/client)
#add_subdirectory(src/widgetgui)
add_subdirectory(src/guibridge)
add_subdirectory(src/qml)
add_subdirectory(src/kirigami)
if(NOT ANDROID)
    add_subdirectory(src/server)
endif()


#enable_testing()
#add_subdirectory(tests/)
