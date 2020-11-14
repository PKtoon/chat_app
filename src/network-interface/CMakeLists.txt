set(NETWORK_INTERFACE
    stream/stream.hpp
    stream/stream.cpp
    connection-manager/connection-manager.hpp
    connection-manager/connection-manager.cpp
    ${CMAKE_SOURCE_DIR}/include/network-interface/netface.hpp
    netface.cpp
    )

add_library("netface" ${NETWORK_INTERFACE})
target_link_libraries(netface ${LIB})
