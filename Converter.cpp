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
            std::cout << "Processing file: " << entry.path() << std::endl;
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << entry.path() << std::endl;
                continue;
            }

            // 解析文件名格式：trace_{pid}_{tid}.raw
            std::string file_name = entry.path().c_str();
            size_t pid_start = file_name.find('_') + 1;
            size_t pid_end = file_name.find('_', pid_start);
            size_t tid_start = pid_end + 1;
            size_t tid_end = file_name.find('.', tid_start);
            
            uint64_t process_id = std::stoull(file_name.substr(pid_start, pid_end - pid_start));
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
            std::cout << "Read " << event_count << " events from " << entry.path() << std::endl;
        }
    }
    std::cout << "Total events read: " << events.size() << std::endl;
    
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
                    visual_events.push_back(visual_event);
                    begin_times.erase(it);
                }
            }
        }
    }
    std::cout << "visual events count " << visual_events.size() << std::endl;
    return visual_events;
}

std::string Converter::to_perfetto_json(const std::vector<VisualEvent>& events) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"traceEvents\": [\n";
    
    // 生成元数据
    json << "    {\n"
         << "      \"ph\": \"M\",\n"
         << "      \"name\": \"process_name\",\n"
         << "      \"cat\": \"__metadata\",\n"
         << "      \"pid\": 1,\n"
         << "      \"ts\": 0,\n"
         << "      \"args\": {\"name\": \"YTracing\"}\n"
         << "    },\n";
    
    // 生成线程元数据
    std::set<uint64_t> thread_ids;
    for (const auto& event : events) {
        thread_ids.insert(std::hash<std::thread::id>{}(event.thread_id));
    }
    
    for (const auto& tid_hash : thread_ids) {
        json << "    {\n"
             << "      \"ph\": \"M\",\n"
             << "      \"name\": \"thread_name\",\n"
             << "      \"cat\": \"__metadata\",\n"
             << "      \"pid\": 1,\n"
             << "      \"tid\": " << tid_hash << ",\n"
             << "      \"ts\": 0,\n"
             << "      \"args\": {\"name\": \"Thread " << tid_hash << "\"}\n"
             << "    },\n";
    }
    
    // 生成事件数据
    for (size_t i = 0; i < events.size(); ++i) {
        const auto& event = events[i];
        json << "    {\n"
             << "      \"ph\": \"X\",\n"
             << "      \"name\": \"" << event.name << "\",\n"
             << "      \"cat\": \"" << event.category << "\",\n"
             << "      \"ts\": " << event.start / 1000 << ",\n"  // 转换为微秒
             << "      \"dur\": " << (event.end - event.start) / 1000 << ",\n"  // 持续时间
             << "      \"pid\": 1,\n"
             << "      \"tid\": " << std::hash<std::thread::id>{}(event.thread_id) << ",\n"
             << "      \"args\": {}\n"
             << "    }";
        
        if (i < events.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "  ]\n}";
    std::cout << json.str() << std::endl;
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

    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, ',')) {
        tokens.push_back(token);
    }

    if (tokens.size() != 3) {
        std::cerr << "Invalid trace format: " << line << "\n";
        is.setstate(std::ios::failbit);
        return is;
    }

    try {
        uint64_t timestamp = std::stoull(tokens[0]);
        event.name = tokens[1];
        event.type = tokens[2] == "B" ? YTracing::EventType::BEGIN : YTracing::EventType::END;
        event.timestamp = std::chrono::high_resolution_clock::time_point(
            std::chrono::nanoseconds(timestamp));
    } catch (const std::exception& e) {
        std::cerr << "Error parsing trace event: " << e.what() << "\n";
        is.setstate(std::ios::failbit);
    }

    return is;
}

} // namespace YTracing
