cmake_minimum_required (VERSION 2.6)

project(MPI_Actor)


# Set up MPI
find_package(MPI REQUIRED)
# Ugly, but works (!)
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(mpicc ${CMAKE_C_COMPILER_ID})


enable_testing()

# Add make check command which outputs error messages from test
add_custom_target( check
    COMMAND ${CMAKE_CTEST_COMMAND} 
        --force-new-ctest-process --output-on-failure
)

add_subdirectory (src)
add_subdirectory (test)
