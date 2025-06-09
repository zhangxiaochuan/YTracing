#include "Converter.h"
#include "Visualizer.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>

namespace fs = std::filesystem;
using namespace YTracing;

std::vector<TraceEvent> read_trace_files(const std::string& trace_dir) {
    std::vector<TraceEvent> events;
    
    for (const auto& entry : fs::directory_iterator(trace_dir)) {
        if (entry.path().extension() == ".raw") {
            std::ifstream in(entry.path());
            TraceEvent event;
            while (in >> event) {
                events.push_back(event);
            }
        }
    }
    
    return events;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <trace_dir>\n";
        return 1;
    }
    
    std::string trace_dir = argv[1];

    // 转换数据
    auto visual_events = Converter::instance().convert(trace_dir);
    
    // 生成Perfetto JSON
    std::string json = Converter::instance().to_perfetto_json(visual_events);
    
    // 生成HTML
    std::string html = Visualizer::instance().generate_html(json);
    
    // 保存Perfetto trace文件
    Converter::instance().save_perfetto_trace(trace_dir, visual_events);
    
    // 保存HTML文件
    Visualizer::instance().save_to_file(html, "trace_visualization.html");
    
    std::cout << "Visualization saved to trace_visualization.html\n";
    return 0;
}
