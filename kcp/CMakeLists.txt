file(GLOB HEADERS "include/kcp/*.h")
add_library( futureio_kcp
    src/ikcp.c
    ${HEADERS})
target_include_directories( futureio_kcp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
