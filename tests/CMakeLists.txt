include_directories(${CMAKE_SOURCE_DIR}/external/SQLite/)

add_executable("clientMessageExchange" "clientMessageExchange.cpp")
target_link_libraries("clientMessageExchange" clientlib )

add_test(NAME clientMessageExchange
    COMMAND clientMessageExchange)
