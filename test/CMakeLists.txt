file(GLOB_RECURSE TEST_SOURCES "*.cpp")
add_executable(rHashGen-tests ${TEST_SOURCES})

target_link_libraries(rHashGen-tests PUBLIC gtest rHashGenLib)

if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0.0)
    target_link_libraries(rhashgen-tests PUBLIC stdc++fs)
endif()


add_test(
    NAME rhashgen:all
    COMMAND rHashGen-tests
)