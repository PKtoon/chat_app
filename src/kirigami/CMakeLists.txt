cmake_minimum_required(VERSION 3.14)

project(kirigamiUI LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/external/SQLite/)

if(NOT ANDROID)
find_package(KF5Kirigami2)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.
#pkpk following section was commented
if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    if (ANDROID_ABI STREQUAL "armeabi-v7a")
        set(ANDROID_EXTRA_LIBS
            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
    endif()
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)


if(ANDROID)
    add_library(kirigamiUI SHARED
        main.cpp
        messagelistmodel.cpp
        messagelistmodel.hpp
        contactlistmodel.cpp
        contactlistmodel.hpp
        guibridge.hpp
        guibridge.cpp
        ../client/client.hpp
        resources.qrc
    )
else()
    add_executable(kirigamiUI
      main.cpp
      messagelistmodel.cpp
      messagelistmodel.hpp
      contactlistmodel.cpp
      contactlistmodel.hpp
      guibridge.hpp
      guibridge.cpp
      ../client/client.hpp
      resources.qrc
    )
endif()

target_compile_definitions(kirigamiUI
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if (ANDROID)
target_link_libraries(kirigamiUI PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick clientlib ${CMAKE_SOURCE_DIR}/src/kirigami/libKF5Kirigami2.so)
elseif()
target_link_libraries(kirigamiUI PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick clientlib KF5::Kirigami2)
endif()
