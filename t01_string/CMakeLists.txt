# Masters string library
add_library(ms_string INTERFACE)

target_include_directories(ms_string INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ms_string INTERFACE ms_common)

add_subdirectory(tests)

