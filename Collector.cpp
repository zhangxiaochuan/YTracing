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
    // 清理并创建tracing目录
    std::filesystem::remove_all("tracing");
    std::filesystem::create_directory("tracing");
    
    // 启动定时flush线程
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
        auto tid_hash = std::hash<std::thread::id>{}(thread_id);
        filename << "tracing/trace_"
                 << thread_events.front().process_id << "_"
                 << tid_hash << ".raw";
        
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
