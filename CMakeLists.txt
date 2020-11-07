cmake_minimum_required(VERSION 2.8)

project(chat_app)

set(CMAKE_CXX_STANDARD 17)
include_directories("external/asio/asio/include/" "external/json/single_include/")

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32_LEAN_AND_MEAN")
endif(WIN32)

set(NETWORK_INTERFACE
    src/stream/stream.h
    src/stream/stream.cpp
    src/connection-manager/connection-manager.h
    src/connection-manager/connection-manager.cpp
    src/network-interface/netface.h
    src/network-interface/netface.cpp
    )

add_library("netface" ${NETWORK_INTERFACE})

set(LIB
    "netface"
    "-lpthread"    
    )
    
if(WIN32)
    set(LIB "${LIB}" "-lws2_32")
endif(WIN32)

    
find_library(PQXX_LIB pqxx)

set(SERVER_SOURCES
    src/server/user.h
    src/server/user.cpp
    src/server/server.h
    src/server/server.cpp
    src/server/main.cpp
    )

add_executable("server" ${SERVER_SOURCES})
target_link_libraries( "server" ${LIB} ${PQXX_LIB})

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

# Find SQLite3
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(SQLITEDB
    external/SQLite/sqlite3_wrap.h
    )

set(CLIENT_SOURCES
    src/client/client.hpp
    src/client/client.cpp
    src/client/newcontactdialog.h
    src/client/newcontactdialog.cpp
    src/client/conndialog.h
    src/client/conndialog.cpp
    src/client/mainwindow.h
    src/client/mainwindow.cpp
    src/client/main.cpp
    )

add_executable("client" ${CLIENT_SOURCES} ${SQLITEDB})
target_link_libraries("client" Qt5::Widgets ${LIB} ${SQLite3_LIBRARIES})
