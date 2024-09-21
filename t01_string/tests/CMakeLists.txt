if (NOT GTest_FOUND)
  return()
endif ()

add_executable(ms_string_tests tests.cpp main.cpp)

target_link_libraries(ms_string_tests
  PRIVATE
    ${GTEST_LIBRARIES}
    pthread
    ms_string
)

gtest_discover_tests(ms_string_tests)

