#ifndef YTRACING_H
#define YTRACING_H

#include <string>
#include <thread>
#include <chrono>
#include <cstdint>

namespace YTracing {

// 事件类型定义
enum class EventType {
    BEGIN,
    END
};

// 事件数据结构
struct TraceEvent {
    std::string name;
    EventType type;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::thread::id thread_id;
    uint64_t process_id;
};

// RAII自动记录器
class AutoTracer {
public:
    explicit AutoTracer(const std::string& name);
    ~AutoTracer();

private:
    static uint64_t get_process_id();
    void record_event(EventType type);

    std::string name_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

// 输入输出操作符重载
std::ostream& operator<<(std::ostream& os, const TraceEvent& event);
std::istream& operator>>(std::istream& is, TraceEvent& event);

} // namespace YTracing

// 宏定义
#define YTRACING_SCOPE(name) YTracing::AutoTracer __tracer_##__LINE__(name)
#define YTRACING_FUNCTION() YTRACING_SCOPE(__FUNCTION__)

#endif // YTRACING_H
