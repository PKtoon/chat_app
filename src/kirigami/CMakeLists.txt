cmake_minimum_required(VERSION 3.14)

project(KirigamiUI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(KF_MIN_VERSION "5.89.0")

#find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

#set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

#include(KDEInstallDirs)
#include(KDECMakeSettings)
#include(KDECompilerSettings NO_POLICY_SCOPE)

#find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 I18n CoreAddons)

find_package(KF5Kirigami2)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    if (ANDROID_ABI STREQUAL "armeabi-v7a")
        set(ANDROID_EXTRA_LIBS
            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
    endif()
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick REQUIRED)

if(ANDROID)
    add_library(KirigamiUI SHARED
        qml.qrc
        ../../resources.qrc
    )
else()
    add_executable(KirigamiUI
      qml.qrc
      ../../resources.qrc
    )
endif()

target_compile_definitions(KirigamiUI
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(KirigamiUI PRIVATE guibridge KF5::Kirigami2)
#target_link_libraries(KirigamiUI PRIVATE guibridge KF5::Kirigami2 KF5::I18n)
