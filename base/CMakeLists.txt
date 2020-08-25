file(GLOB HEADERS "include/base/*.h")
add_library( futureio_base
        src/Hex.cpp
        src/StringUtils.cpp
        ${HEADERS} )

target_link_libraries( futureio_base )
target_include_directories( futureio_base PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
