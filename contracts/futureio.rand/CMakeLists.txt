
file(GLOB ABI_FILES "*.abi")
configure_file("${ABI_FILES}" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

file(GLOB WASM_FILES "futureio.rand.wasm.m")
configure_file("${WASM_FILES}" "${CMAKE_CURRENT_BINARY_DIR}/futureio.rand.wasm" COPYONLY)

file(GLOB WAST_FILES "futureio.rand.wast.t")
configure_file("${WAST_FILES}" "${CMAKE_CURRENT_BINARY_DIR}/futureio.rand.wast" COPYONLY)
