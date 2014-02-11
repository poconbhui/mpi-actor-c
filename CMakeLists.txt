cmake_minimum_required (VERSION 2.6)

project(MPI_Actor)


# Set up MPI
find_package(MPI REQUIRED)
# Ugly, but works (!)
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(mpicc ${CMAKE_C_COMPILER_ID})



# Add make check command which outputs error messages from test
add_custom_target( check
    COMMAND ${CMAKE_CTEST_COMMAND} 
        --force-new-ctest-process --output-on-failure
)


include_directories(${CMAKE_SOURCE_DIRECTORY}/src)


add_subdirectory (src)


# Enable testing from here
enable_testing()
add_subdirectory (test)
