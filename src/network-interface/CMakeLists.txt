if(NOT ANDROID)
    set(LIB "-pthread")
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32_LEAN_AND_MEAN")
    set(LIB "${LIB}" "-lws2_32")
endif(WIN32)

set(NETWORK_INTERFACE
    stream/stream.hpp
    stream/stream.cpp
    connection-manager/connection-manager.hpp
    connection-manager/connection-manager.cpp
    netface.hpp
    netface.cpp
    )

add_library("netface" ${NETWORK_INTERFACE})
target_link_libraries(netface ${LIB})
