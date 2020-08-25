file(GLOB HEADERS "include/lightclient/*.h")

add_library( futureio_lightclient
        src/CheckPoint.cpp
        src/CommitteeDelta.cpp
        src/CommitteeInfo.cpp
        src/CommitteeSet.cpp
        src/ConfirmPoint.cpp
        src/EpochEndPoint.cpp
        src/Helper.cpp
        src/LightClient.cpp
        src/LightClientCallback.cpp
        src/LightClientMgr.cpp
        src/LightClientProducer.cpp
        ${HEADERS} )

target_link_libraries( futureio_lightclient futureio_core )
target_include_directories( futureio_lightclient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
