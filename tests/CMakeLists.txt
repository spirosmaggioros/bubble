file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/tests/*.cc"
        "${CMAKE_SOURCE_DIR}/tests/test_main.cc")

add_executable(runUnitTests ${TEST_SOURCES})

target_link_libraries(runUnitTests PUBLIC ${CMAKE_DL_LIBS})
enable_testing()

add_test(NAME runUnitTests COMMAND runUnitTests)
