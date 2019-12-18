cmake_minimum_required(VERSION 2.8)

project(chat_app)

include_directories("~/boost/usr/include")
link_directories("~/boost/usr/lib")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lboost_serialization")

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

#file(GLOB SERVER_SOURCES "server/*.h" "server/*.cpp")
set(SERVER_SOURCES
    src/server/user.h
    src/server/user.cpp
    src/server/server.h
    src/server/server.cpp
    src/server/main.cpp
    )

#file(GLOB CLIENT_SOURCES "client/*.h" "client/*.cpp")
set(CLIENT_SOURCES
    src/client/client.h
    src/client/client.cpp
    src/client/main.cpp
    )



#file( GLOB LIB "src/stream/*" )
set(STREAM
    src/stream/stream.h
    src/stream/stream.cpp
    )

add_library( "stream" ${STREAM} )

add_executable("server"  ${SERVER_SOURCES})
add_executable("client" ${CLIENT_SOURCES} )

target_link_libraries( "server" "stream" )
target_link_libraries( "client" "stream" )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(CLIENT_GUI_SOURCES
    src/client-gui/connectionmanager.h
    src/client-gui/connectionmanager.cpp
    src/client-gui/netface.h
    src/client-gui/netface.cpp
    src/client-gui/mainwindow.h
    src/client-gui/mainwindow.cpp
    src/client-gui/main-gui.cpp
    )

    add_executable("client-gui" ${CLIENT_GUI_SOURCES} )
    target_link_libraries( "client-gui" "stream" Qt5::Widgets )
