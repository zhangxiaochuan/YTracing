add_executable(YViewer YViewer.cpp)

target_link_libraries(YViewer PRIVATE YTracingVisualizer)

install(TARGETS YViewer DESTINATION ${CMAKE_INSTALL_BINDIR}) # 安装到 /usr/local/bin/