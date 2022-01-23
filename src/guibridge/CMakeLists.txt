cmake_minimum_required(VERSION 3.14)

project(guibridge LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/external/SQLite/)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Multimedia REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Multimedia REQUIRED)

add_library(guibridge
    main.cpp
    messagelistmodel.cpp
    messagelistmodel.hpp
    contactlistmodel.cpp
    contactlistmodel.hpp
    guibridge.hpp
    guibridge.cpp
    ../client/client.hpp
    )

target_link_libraries(guibridge
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Multimedia clientlib)
