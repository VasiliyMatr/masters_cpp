add_executable(ms_string_tests tests.cpp main.cpp)

target_link_libraries(ms_string_tests
  PRIVATE
    GTest::gtest_main
    GTest::gtest
    pthread
    ms_string
)

include(GoogleTest)
gtest_discover_tests(ms_string_tests)
