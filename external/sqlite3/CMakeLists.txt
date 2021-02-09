
set(SQLITE_SOURCES
    shell.c
    sqlite3.c
    )

add_library("sqlite" ${SQLITE_SOURCES})
if(NOT ANDROID)
target_link_libraries( "sqlite" "-pthread" "-ldl")
endif()
