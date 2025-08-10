add_library(YTracingCore
    YTracing.h
    Collector.h
    AutoTracer.cpp
    Collector.cpp
)

target_include_directories(YTracingCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(YTracingCore PUBLIC
    pthread
    stdc++fs
)

add_library(YTracingVisualizer
    Converter.h
    Converter.cpp
)

target_include_directories(YTracingVisualizer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(YTracingVisualizer PUBLIC
    YTracingCore
    stdc++fs
)