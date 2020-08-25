add_executable( keypair_generator
        KeyPairGenerator.cpp)
target_link_libraries( keypair_generator futureio_ed25519 )

add_executable( ws2json ws2json.cpp)
target_include_directories( ws2json PRIVATE ${Boost_INCLUDE_DIR} )
target_link_libraries( ws2json PRIVATE fc futureio_chain chainbase)

add_executable( bls_keypair_gen
        BlsKeyPairGenerator.cpp)
target_link_libraries( bls_keypair_gen futureio_base futureio_crypto )

add_executable( committee_sk_gen
        CommitteeSkGenerator.cpp)
target_link_libraries( committee_sk_gen futureio_base futureio_crypto futureio_chain)

add_executable( gm_keys_gen
        GmKeysGenerator.cpp)
target_link_libraries( gm_keys_gen futureio_gm )
