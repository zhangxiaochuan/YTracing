#include "Converter.h"
#include <set>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>


namespace YTracing {

Converter& Converter::instance() {
    static Converter converter;
    return converter;
}

std::vector<VisualEvent> Converter::convert(const std::string& trace_dir) {
    std::vector<TraceEvent> events;
    
    // 读取目录下所有trace文件
    for (const auto& entry : std::filesystem::directory_iterator(trace_dir)) {
        if (entry.path().extension() == ".raw") {
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << entry.path() << std::endl;
                continue;
            }

            // 解析文件名格式：trace_{pid}_{tid}.raw
            std::string file_name = entry.path().stem().c_str();
            size_t pid_start = file_name.find('_') + 1;
            size_t pid_end = file_name.find('_', pid_start);
            size_t tid_start = pid_end + 1;
            size_t tid_end = file_name.find('.', tid_start);
            
            int32_t process_id = std::stoull(file_name.substr(pid_start, pid_end - pid_start));
            uint64_t tid_value = std::stoull(file_name.substr(tid_start, tid_end - tid_start));
            std::thread::id thread_id = std::thread::id(tid_value);

            TraceEvent event;
            event.process_id = process_id;
            event.thread_id = thread_id;
            size_t event_count = 0;
            while (file >> event) {
                if (file.fail()) {
                    std::cerr << "Failed to parse event in file: " << entry.path() << std::endl;
                    file.clear();
                    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
                events.push_back(event);
                event_count++;
            }
        }
    }
    
    if (events.empty()) {
        std::cerr << "Warning: No valid events found in trace files\n";
        return {};
    }

    std::vector<VisualEvent> visual_events;
    auto grouped_events = group_by_thread(events);
    
    // 验证事件对是否完整
    std::map<std::string, int> event_pairs;
    for (const auto& event : events) {
        if (event.type == EventType::BEGIN) {
            event_pairs[event.name]++;
        } else if (event.type == EventType::END) {
            event_pairs[event.name]--;
        }
    }

    // 检查未匹配的事件
    for (const auto& [name, count] : event_pairs) {
        if (count != 0) {
            std::cerr << "Warning: Unmatched events for " << name 
                     << " (count: " << count << ")\n";
        }
    }

    for (auto& [thread_id, thread_events] : grouped_events) {
        std::map<std::string, uint64_t> begin_times;
        
        for (const auto& event : thread_events) {
            if (event.type == EventType::BEGIN) {
                begin_times[event.name] = event.timestamp.time_since_epoch().count();
            } else if (event.type == EventType::END) {
                auto it = begin_times.find(event.name);
                if (it != begin_times.end()) {
                    VisualEvent visual_event;
                    visual_event.name = event.name;
                    visual_event.category = "default";
                    visual_event.start = it->second;
                    visual_event.end = event.timestamp.time_since_epoch().count();
                    visual_event.thread_id = thread_id;
                    visual_event.process_id = event.process_id;
                    visual_events.push_back(visual_event);
                    begin_times.erase(it);
                }
            }
        }
    }
    return visual_events;
}

    
std::string Converter::to_perfetto_json(const std::vector<VisualEvent>& events) {
    if (events.empty()) {
        return "{\"traceEvents\": []}";
    }

    std::ostringstream json;
    json << "{\n";
    json << "  \"traceEvents\": [\n";

    bool first_entry = true;

    // --- 0. 按时间顺序映射tid ---
    // 记录每个thread第一次出现的时间
    std::unordered_map<std::thread::id, uint64_t> thread_first_ts;
    for (const auto& event : events) {
        if (thread_first_ts.find(event.thread_id) == thread_first_ts.end()) {
            thread_first_ts[event.thread_id] = event.start; // 第一次出现的时间戳
        } else {
            if (event.start < thread_first_ts[event.thread_id]) {
                thread_first_ts[event.thread_id] = event.start;
            }
        }
    }

    // 把(thread_id, first_ts)放到vector里排序
    std::vector<std::pair<std::thread::id, uint64_t>> tids_sorted(thread_first_ts.begin(), thread_first_ts.end());
    std::sort(tids_sorted.begin(), tids_sorted.end(),
              [](const auto& a, const auto& b) {
                  return a.second < b.second; // 按首次出现时间升序
              });

    // 生成tid映射
    std::unordered_map<std::thread::id, uint32_t> tid_map;
    uint32_t next_tid = 0;
    for (const auto& [tid, _] : tids_sorted) {
        tid_map[tid] = next_tid++;
    }

    // --- 1. 动态生成元数据 ---
    std::map<uint64_t, std::string> process_names;
    std::map<uint64_t, std::string> thread_names;
    std::map<uint64_t, uint64_t> thread_to_process_map;

    for(const auto& event : events) {
        process_names[event.process_id] = "Process " + std::to_string(event.process_id);
        thread_names[tid_map[event.thread_id]] = "Thread " + std::to_string(tid_map[event.thread_id]);
        thread_to_process_map[tid_map[event.thread_id]] = event.process_id;
    }

    // 为每个进程生成 "process_name" 元数据
    for (const auto& [pid, name] : process_names) {
        if (!first_entry) json << ",\n";
        json << "    {\"ph\": \"M\", \"name\": \"process_name\", \"cat\": \"__metadata\", \"pid\": " << pid
             << ", \"ts\": 0}";
        first_entry = false;
    }

    // 为每个线程生成 "thread_name" 元数据
    for (const auto& [tid, name] : thread_names) {
        json << ",\n";
        json << "    {\"ph\": \"M\", \"name\": \"thread_name\", \"cat\": \"__metadata\", \"pid\": " << thread_to_process_map[tid]
             << ", \"tid\": " << tid << ", \"ts\": 0}";
    }
    json << ",\n";

    // --- 2. 生成事件数据 ---
    for (size_t i = 0; i < events.size(); ++i) {
        const auto& event = events[i];
        json << "    {\n"
             << "      \"ph\": \"X\",\n"
             << "      \"name\": \"" << event.name << "\",\n"
             << "      \"cat\": \"" << event.category << "\",\n"
             << "      \"ts\": " << event.start / 1000 << ",\n"  // 转换为微秒
             << "      \"dur\": " << (event.end - event.start) / 1000 << ",\n"
             << "      \"pid\": " << event.process_id << ",\n"
             << "      \"tid\": " << tid_map[event.thread_id] << ",\n"
             << "      \"args\": {}\n"
             << "    }";

        if (i < events.size() - 1) {
            json << ",";
        }
        json << "\n";
    }

    json << "  ]\n}";
    return json.str();
}


void Converter::save_perfetto_trace(const std::string& trace_dir, 
                                  const std::vector<VisualEvent>& events) {
    std::ofstream out(trace_dir + "/trace.json");
    if (out.is_open()) {
        out << to_perfetto_json(events);
    }
}

std::map<std::thread::id, std::vector<TraceEvent>> 
Converter::group_by_thread(const std::vector<TraceEvent>& events) {
    std::map<std::thread::id, std::vector<TraceEvent>> grouped_events;
    
    for (const auto& event : events) {
        grouped_events[event.thread_id].push_back(event);
    }
    
    return grouped_events;
}

std::istream& operator>>(std::istream& is, YTracing::TraceEvent& event) {
    std::string line;
    if (!std::getline(is, line)) {
        return is;
    }

    auto first_comma = line.find(',');
    auto last_comma = line.rfind(',');

    if (first_comma == std::string::npos || last_comma == std::string::npos ||
        first_comma == last_comma) {
        std::cerr << "Invalid trace format: " << line << "\n";
        is.setstate(std::ios::failbit);
        return is;
    }

    try {
        uint64_t timestamp = std::stoull(line.substr(0, first_comma));
        event.name = line.substr(first_comma + 1, last_comma - first_comma - 1);
        std::string type_str = line.substr(last_comma + 1);

        event.type = type_str == "B" ? YTracing::EventType::BEGIN
                                      : YTracing::EventType::END;
        event.timestamp = std::chrono::high_resolution_clock::time_point(
            std::chrono::nanoseconds(timestamp));
    } catch (const std::exception& e) {
        std::cerr << "Error parsing trace event: " << e.what() << "\n";
        is.setstate(std::ios::failbit);
    }

    return is;
}

} // namespace YTracing
