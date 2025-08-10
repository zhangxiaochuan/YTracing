#include "Collector.h"
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <ctime>

namespace YTracing {

Collector& Collector::instance() {
    static Collector collector;
    return collector;
}

Collector::Collector() {
    // 1. 创建带时间戳的目录名
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "tracing_" << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    trace_dir_ = ss.str();

    // 2. 创建目录
    std::filesystem::create_directory(trace_dir_);
    
    // 3. 启动后台刷新线程
    flush_thread_ = std::thread(&Collector::flush_loop, this);
}

void Collector::flush_loop() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        flush_to_file();
    }
}

Collector::~Collector() {
    running_ = false;
    if (flush_thread_.joinable()) {
        flush_thread_.join();
    }
    flush_to_file();
}

void Collector::add_event(const TraceEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    events_[event.thread_id].push_back(event);
}

void Collector::flush_to_file() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [thread_id, thread_events] : events_) {
        // 生成文件名
        std::ostringstream filename;
        filename << trace_dir_ << "/trace_"
                 << thread_events.front().process_id << "_"
                 << thread_id << ".raw";
        
        // 写入文件
        std::ofstream out(filename.str(), std::ios::app);
        if (out.is_open()) {
            for (const auto& event : thread_events) {
                out << std::chrono::duration_cast<std::chrono::nanoseconds>(
                    event.timestamp.time_since_epoch()).count() << ","
                    << event.name << ","
                    << (event.type == EventType::BEGIN ? "B" : "E") << "\n";
            }
        }
    }
    
    // 清空已写入的事件
    events_.clear();
}

} // namespace YTracing
