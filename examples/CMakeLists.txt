add_definitions(-DTRACING=1)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE YTracingCore)