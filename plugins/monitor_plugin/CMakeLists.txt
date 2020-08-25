file(GLOB HEADERS "include/futureio/monitor_plugin/*.hpp")
add_library( monitor_plugin
             monitor_plugin.cpp
             httpc_only_send.cpp
             ${HEADERS} )

target_link_libraries( monitor_plugin appbase fc http_plugin futureio_rpos)
target_include_directories( monitor_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
