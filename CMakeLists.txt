cmake_minimum_required(VERSION 2.8)

project(chat_app)

include_directories("external/asio/asio/include/" "external/json/single_include/")

set(STREAM
    src/stream/stream.h
    src/stream/stream.cpp
    )

add_library("stream" ${STREAM})

set(LIB
    "stream"
    "-lpthread"
    )

set(NETWORK_INTERFACE
    src/connection-manager/connection-manager.h
    src/connection-manager/connection-manager.cpp
    src/network-interface/netface.h
    src/network-interface/netface.cpp
    )
    
set(SERVER_SOURCES
    src/server/user.h
    src/server/user.cpp
    src/server/server.h
    src/server/server.cpp
    src/server/main.cpp
    )

add_executable("server" ${SERVER_SOURCES} ${NETWORK_INTERFACE})
target_link_libraries( "server" ${LIB})

set(CLIENT_SOURCES
    src/client/client.h
    src/client/client.cpp
    src/client/main.cpp
    )
    
add_executable("client" ${CLIENT_SOURCES} ${NETWORK_INTERFACE})
target_link_libraries("client" ${LIB})


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

set(CLIENT_GUI_SOURCES
    src/client-gui/newcontactdialog.h
    src/client-gui/newcontactdialog.cpp
    src/client-gui/conndialog.h
    src/client-gui/conndialog.cpp
    src/client-gui/mainwindow.h
    src/client-gui/mainwindow.cpp
    src/client-gui/main-gui.cpp
    )

add_executable("client-gui" ${CLIENT_GUI_SOURCES} ${NETWORK_INTERFACE} ${SQLITEDB})
target_link_libraries("client-gui" Qt5::Widgets ${LIB} ${SQLite3_LIBRARIES})
