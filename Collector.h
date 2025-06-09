#ifndef YTRACING_COLLECTOR_H
#define YTRACING_COLLECTOR_H

#include "YTracing.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>

namespace YTracing {

class Collector {
public:
    static Collector& instance();
    
    void add_event(const TraceEvent& event);
    void flush_to_file();

private:
    Collector();
    ~Collector();

    std::mutex mutex_;
    std::unordered_map<std::thread::id, std::vector<TraceEvent>> events_;
    std::thread flush_thread_;
    std::atomic<bool> running_{true};
    void flush_loop();
};

} // namespace YTracing

#endif // YTRACING_COLLECTOR_H
