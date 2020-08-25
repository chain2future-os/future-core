file(GLOB HEADERS "include/rpos/*.h")

add_library( futureio_rpos
        src/BlockMsgPool.cpp
        src/Config.cpp
        src/EvilBlsDetector.cpp
        src/EvilDDosDetector.cpp
        src/EvilMultiProposeDetector.cpp
        src/EvilMultiVoteDetector.cpp
        src/FisherYates.cpp
        src/Genesis.cpp
        src/MsgBuilder.cpp
        src/MsgMgr.cpp
        src/NativeTrx.cpp
        src/Node.cpp
        src/NodeInfo.cpp
        src/Proof.cpp
        src/PunishMgr.cpp
        src/RoleRandom.cpp
        src/RoleSelection.cpp
        src/Scheduler.cpp
        src/Seed.cpp
        src/StakeVoteBase.cpp
        src/StakeVoteFactory.cpp
        src/StakeVoteRandom.cpp
        src/StakeVoteVrf.cpp
        src/SyncTask.cpp
        src/Utils.cpp
        src/VoterSet.cpp
        src/Vrf.cpp
        ${HEADERS} )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=delete-non-virtual-dtor" )

target_link_libraries( futureio_rpos net_plugin kcp_plugin futureio_lightclient futureio_core futureio_crypto )
target_include_directories( futureio_rpos PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
