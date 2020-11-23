include_directories(${CMAKE_SOURCE_DIR}/external/SQLite/)

# Find SQLite3
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

set(CLIENT_SOURCES
    client.hpp
    client.cpp
    )

add_library(clientlib ${CLIENT_SOURCES})
target_link_libraries(clientlib netface ${SQLite3_LIBRARIES})

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(GUI_SOURCES
    newcontactdialog.hpp
    newcontactdialog.cpp
    conndialog.hpp
    conndialog.cpp
    mainwindow.hpp
    mainwindow.cpp
    main.cpp
    )

add_executable("client" ${GUI_SOURCES})
target_link_libraries("client" Qt5::Widgets clientlib)
