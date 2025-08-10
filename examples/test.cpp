#include "YTracing.h"
#include <thread>
#include <chrono>

void test_function() {
    YTRACING_FUNCTION();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main() {
    YTRACING_FUNCTION();
    
    {
        YTRACING_SCOPE("Main Scope");
        std::thread t1(test_function);
        std::thread t2(test_function);
        
        t1.join();
        t2.join();
    }
    
    return 0;
}
