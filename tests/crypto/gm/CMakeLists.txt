#GM profile test
add_executable( gmprofile_test
        GmProfileTest.cpp)

target_link_libraries( gmprofile_test futureio_gm futureio_ed25519)
