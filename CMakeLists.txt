cmake_minimum_required (VERSION 2.6)

project(MPI_Actor)


# Set up MPI
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories("${MPI_INCLUDE_PATH}")


enable_testing()

# Add make check command which outputs error messages from test
add_custom_target( check
    COMMAND ${CMAKE_CTEST_COMMAND} 
        --force-new-ctest-process --output-on-failure
)

add_subdirectory (src)
add_subdirectory (test)
