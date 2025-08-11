#ifndef YTRACING_CONVERTER_H
#define YTRACING_CONVERTER_H

#include "YTracing.h"
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

namespace YTracing {

struct VisualEvent {
    std::string name;
    std::string category;
    uint64_t start;
    uint64_t end;
    std::thread::id thread_id;
    int32_t process_id;
    uint64_t parent_id;  // 父事件ID
    uint64_t event_id;   // 当前事件ID
};

class Converter {
public:
    static Converter& instance();
    
    // 将原始trace数据转换为可视化格式
    std::vector<VisualEvent> convert(const std::string& trace_dir);
    
    // 生成Perfetto兼容的JSON格式
    std::string to_perfetto_json(const std::vector<VisualEvent>& events);
    
    // 生成完整的Perfetto trace文件
    void save_perfetto_trace(const std::string& trace_dir, 
                           const std::vector<VisualEvent>& events);

private:
    Converter() = default;

    // 将事件按线程分组
    std::map<std::thread::id, std::vector<TraceEvent>> group_by_thread(
        const std::vector<TraceEvent>& events);
};

} // namespace YTracing

#endif // YTRACING_CONVERTER_H
