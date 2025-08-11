add_executable(YViewer YViewer.cpp)
set_target_properties(YViewer PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)
target_link_libraries(YViewer PRIVATE YTracingVisualizer)

install(TARGETS YViewer DESTINATION ${CMAKE_INSTALL_BINDIR}) # 安装到 /usr/local/bin/