#include_directories(${CMAKE_SOURCE_DIR}/external/sqlite3/)

set(CLIENT_SOURCES
    sqlite3_wrap.h
    client.hpp
    client.cpp
    )

add_library(clientlib ${CLIENT_SOURCES})
target_link_libraries(clientlib netface sqlite)
