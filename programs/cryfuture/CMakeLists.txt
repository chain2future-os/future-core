add_executable( cryfuture main.cpp )

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling kfutured with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

target_link_libraries( cryfuture 
        PRIVATE fc ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )
