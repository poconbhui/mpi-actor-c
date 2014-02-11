add_library(mpitest mpitest.c)


include_directories(${CMAKE_SOURCE_DIR}/src)


# Add tests
#
# Expect test files in form myfeature.test.c
#
FILE(GLOB TESTFILES *.test.c)

foreach(TESTFILE ${TESTFILES})
    GET_FILENAME_COMPONENT(TEST ${TESTFILE} NAME_WE)

    MESSAGE("-- Adding test: ${TEST}.test")

    add_executable(${TEST}.test ${TEST}.test.c)
    target_link_libraries(${TEST}.test actor mpitest)
    add_test(${TEST}.test ${TEST}.test)
endforeach(TESTFILE)
