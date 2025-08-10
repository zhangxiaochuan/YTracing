cmake_minimum_required(VERSION 3.10)
project(YTracing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(YTracing
    YTracing.h
    Collector.h
    AutoTracer.cpp
    Collector.cpp
)

add_library(YTracingVisualizer
    Converter.h
    Converter.cpp
)

add_definitions(-DTRACING=1)
# 添加可执行文件
add_executable(main
    main.cpp
)

add_executable(YViewer
    main_visualizer.cpp
)


# 添加链接库
target_link_libraries(main PRIVATE pthread stdc++fs YTracing)
target_link_libraries(YViewer PRIVATE pthread stdc++fs YTracingVisualizer)
