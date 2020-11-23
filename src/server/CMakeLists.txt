find_library(PQXX_LIB pqxx)

set(SERVER_SOURCES
    psql-wrap.hpp
    user.hpp
    user.cpp
    server.hpp
    server.cpp
    main.cpp
    )

add_executable("server" ${SERVER_SOURCES})
target_link_libraries( "server" netface ${PQXX_LIB})
