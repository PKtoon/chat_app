include_directories(${CMAKE_SOURCE_DIR}/external/SQLite/)

add_executable("clientMessageExchange" "clientMessageExchange.cpp")
target_link_libraries("clientMessageExchange" clientlib ${SQLite3_LIBRARIES})

add_test(NAME clientMessageExchange
    COMMAND clientMessageExchange)
