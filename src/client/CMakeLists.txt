include_directories(${CMAKE_SOURCE_DIR}/external/SQLite/)

set(CLIENT_SOURCES
    client.hpp
    client.cpp
    )

add_library(clientlib ${CLIENT_SOURCES})
target_link_libraries(clientlib netface sqlite)

if(NOT ANDROID)
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

endif()
