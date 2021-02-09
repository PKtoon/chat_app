include_directories(${CMAKE_SOURCE_DIR}/external/SQLite/)

set(CLIENT_SOURCES
    client.hpp
    client.cpp
    )

add_library(clientlib ${CLIENT_SOURCES})
target_link_libraries(clientlib netface sqlite)
