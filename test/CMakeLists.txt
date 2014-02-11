# Set up MPI
find_package(MPI REQUIRED)
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(mpicc ${CMAKE_C_COMPILER_ID})


add_library( mpitest
    "${PROJECT_SOURCE_DIR}/test/mpitest.c"
)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/test")


add_executable(actor_create.test actor_create.test.c)
target_link_libraries(actor_create.test actor mpitest)
add_test(actor_create.test actor_create.test)
