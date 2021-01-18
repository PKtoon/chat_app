find_library(PQXX_LIB pqxx)

set(SERVER_LIB netface ${PQXX_LIB})

if(WIN32)
    set(SERVER_LIB "${SERVER_LIB}" "-lpq" "-lwsock32")
endif(WIN32)

set(SERVER_SOURCES
    psql-wrap.hpp
    user.hpp
    user.cpp
    server.hpp
    server.cpp
    main.cpp
    )

add_executable("server" ${SERVER_SOURCES})
target_link_libraries( "server" ${SERVER_LIB})
