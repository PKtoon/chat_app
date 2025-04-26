if(NOT ANDROID)
# Find the QtWidgets library
# find_package(Qt5Widgets CONFIG REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Multimedia Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Multimedia Widgets REQUIRED)


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
target_link_libraries("client" clientlib Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Widgets)

endif()
