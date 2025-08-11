add_library(YTracingCore
    YTracing.h
    Collector.h
    AutoTracer.cpp
    Collector.cpp
)
set_target_properties(YTracingCore PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
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
set_target_properties(YTracingVisualizer PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)
target_include_directories(YTracingVisualizer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(YTracingVisualizer PUBLIC
    YTracingCore
    stdc++fs
)

include(CMakePackageConfigHelpers)

# 安装库文件
install(TARGETS YTracingCore YTracingVisualizer
        EXPORT YTracingTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装所有公开的头文件
install(FILES
        YTracing.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/YTracing # 安装到 /usr/local/include/YTracing/
)

# 从模板生成 YTracingConfig.cmake
configure_package_config_file(
        "YTracingConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/YTracingConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/YTracing
)

# 安装生成的 YTracingConfig.cmake
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/YTracingConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/YTracing
)

# 安装导出的目标文件 (YTracingTargets.cmake)
install(EXPORT YTracingTargets
        FILE YTracingTargets.cmake
        NAMESPACE YTracing:: # 为目标添加命名空间
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/YTracing
)