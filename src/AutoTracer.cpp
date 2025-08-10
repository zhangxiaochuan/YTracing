#include "YTracing.h"
#include "Collector.h"
#include <unistd.h>

namespace YTracing {

AutoTracer::AutoTracer(const std::string& name)
    : name_(name), start_time_(std::chrono::high_resolution_clock::now()) {
    record_event(EventType::BEGIN);
}

AutoTracer::~AutoTracer() {
    record_event(EventType::END);
}

uint64_t AutoTracer::get_process_id() {
    return static_cast<uint64_t>(::getpid());
}

void AutoTracer::record_event(EventType type) {
    Collector::instance().add_event({
        name_,
        type,
        std::chrono::high_resolution_clock::now(),
        std::this_thread::get_id(),
        get_process_id()
    });
}

} // namespace YTracing
