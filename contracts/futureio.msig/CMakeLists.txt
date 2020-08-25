
file(GLOB ABI_FILES "*.abi")
configure_file("${ABI_FILES}" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

add_wast_executable(TARGET futureio.msig
  INCLUDE_FOLDERS "${STANDARD_INCLUDE_FOLDERS}"
  LIBRARIES libc++ libc futureiolib
  DESTINATION_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
)
