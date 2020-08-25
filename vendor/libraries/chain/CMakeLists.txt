configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/futureio/chain/core_symbol.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/futureio/chain/core_symbol.hpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/genesis_state_root_key.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/genesis_state_root_key.cpp)

file(GLOB HEADERS "include/futureio/chain/*.hpp"
                  "include/futureio/chain/webassembly/*.hpp"
                  "${CMAKE_CURRENT_BINARY_DIR}/include/futureio/chain/core_symbol.hpp" )

## SORT .cpp by most likely to change / break compile
add_library( futureio_chain
             merkle.cpp
             name.cpp
             name_ex.cpp
             transaction.cpp
             block_header.cpp
             block_header_state.cpp
             block_state.cpp
             fork_database.cpp
             controller.cpp
             authorization_manager.cpp
             resource_limits.cpp
             block_log.cpp
             transaction_context.cpp
             futureio_contract.cpp
             futureio_contract_abi.cpp
             chain_config.cpp
             chain_id_type.cpp
             callback.cpp
             callback_manager.cpp
             genesis_state.cpp
             ${CMAKE_CURRENT_BINARY_DIR}/genesis_state_root_key.cpp

#             chain_config.cpp
#             block_trace.cpp
              wast_to_wasm.cpp
              wasm_interface.cpp
              wasm_futureio_validation.cpp
              wasm_futureio_injection.cpp
              apply_context.cpp
              abi_serializer.cpp
              asset.cpp

             webassembly/wavm.cpp
             webassembly/binaryen.cpp

             worldstate
             worldstate_file_manager

             webassembly/wabt.cpp
             config.cpp
             block_timestamp.cpp
             bls_votes.cpp
#             get_config.cpp
#             global_property_object.cpp
#
#             contracts/chain_initializer.cpp


#             transaction_metadata.cpp
             ${HEADERS}
             )

add_dependencies(futureio_chain  futureio.token)
if(ENABLE_ZKP)
  target_link_libraries( futureio_chain future_utilities futureio_crypto futureio_gm fc chainbase appbase chain_plugin Logging IR WAST WASM Runtime
      wasm asmjs passes cfg ast emscripten-optimizer support softfloat builtins wabt
      "${CMAKE_CURRENT_SOURCE_DIR}/../../snarkv/libs/libsnarkv.a"
      "${CMAKE_CURRENT_SOURCE_DIR}/../../snarkv/libs/libsnark.a"
      "${CMAKE_CURRENT_SOURCE_DIR}/../../snarkv/libs/libgmp.a"
      "${CMAKE_CURRENT_SOURCE_DIR}/../../snarkv/libs/libgmpxx.a"
      "${CMAKE_CURRENT_SOURCE_DIR}/../../snarkv/libs/librustzcash.a"
      "${CMAKE_CURRENT_SOURCE_DIR}/../../snarkv/libs/libsodium.a")
else()
  target_link_libraries( futureio_chain future_utilities futureio_crypto futureio_gm fc chainbase appbase chain_plugin Logging IR WAST WASM Runtime
      wasm asmjs passes cfg ast emscripten-optimizer support softfloat builtins wabt )
endif()

target_include_directories( futureio_chain
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/../wasm-jit/Include"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/../../../vendor/binaryen/src"
                                   "${CMAKE_BINARY_DIR}/contracts"
if(ENABLE_ZKP)
                                   "${CMAKE_CURRENT_SOURCE_DIR}/../../snarkv/include"
endif()
                                   "${CMAKE_SOURCE_DIR}/vendor/libraries/wabt"
                                   "${CMAKE_BINARY_DIR}/vendor/libraries/wabt"
                            )

install( TARGETS futureio_chain
   RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
)
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/futureio/chain/
      DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/futureio/chain
      FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "webassembly" EXCLUDE
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/futureio/chain/core_symbol.hpp DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/futureio/chain)
#if(MSVC)
#  set_source_files_properties( db_init.cpp db_block.cpp database.cpp block_log.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
#endif(MSVC)
