
add_library(ms_common INTERFACE)

target_include_directories(ms_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ms_common INTERFACE fmt::fmt)
