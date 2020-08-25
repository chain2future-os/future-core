file(GLOB HEADERS "include/futureio/producer_rpos_plugin/*.hpp")
add_library( producer_rpos_plugin
             producer_rpos_plugin.cpp
             ${HEADERS} )

target_link_libraries( producer_rpos_plugin chain_plugin http_client_plugin futureio_chain future_utilities net_plugin appbase fc futureio_rpos futureio_core )
target_include_directories( producer_rpos_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
