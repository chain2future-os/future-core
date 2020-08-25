file(GLOB HEADERS "include/p2p/*.h")
add_library( futureio_p2p
    src/NodeTable.cpp
    src/UPnP.cpp
    ${HEADERS} )
include_directories( "${PROJECT_SOURCE_DIR}/vendor/miniupnpc" )
target_link_libraries( futureio_p2p futureio_core appbase miniupnpc-private libminiupnpc-static futureio_kcp)
target_include_directories( futureio_p2p PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/appbase/include")
