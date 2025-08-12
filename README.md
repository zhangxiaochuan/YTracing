[English](README.md) | [中文](README_zh.md)

# YTracing

YTracing is a high-performance C++ event tracing library, supporting performance analysis and visualization in multi-threaded and multi-process environments.

---

## Features

- Lightweight and low-overhead performance tracing  
- Supports multi-thread and multi-process event recording  
- Generates visualization data compatible with [Perfetto](https://perfetto.dev/)  
- Provides CLI tools for data processing and analysis  

---

## Installation

### Method 1: Install via apt

1. Add the PPA repository:
```bash
sudo add-apt-repository ppa:zhangxiaochuan/ytracing
sudo apt update
````

2. Install the development package (includes headers and libraries):

```bash
sudo apt install libytracing-dev
```

### Method 2: Build from source

1. Clone the repository:

```bash
git clone https://github.com/zhangxiaochuan/YTracing.git
cd YTracing
```

2. Build and install:

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

---

## Usage

### Enable tracing

Add the following definition in `CMakeLists.txt` to enable tracing:

```cmake
add_definitions(-DTRACING=1)
```

Include the header file and use the following two methods to record tracing information:
```cpp
#include "YTracing.h"

void my_function() {
    // Method 1: YTRACING_FUNCTION
    // Records the execution time of the entire function (scope is the whole function)
    YTRACING_FUNCTION();

    // Simulate code execution
    for (int i = 0; i < 1000000; ++i) {}

    {
        // Method 2: YTRACING_SCOPE("Custom Scope Name")
        // Records the execution time of a custom code block (scope is limited to the braces)
        YTRACING_SCOPE("Inner Loop Work");

        for (int j = 0; j < 500000; ++j) {}
    }
}
```
Difference:

* YTRACING_FUNCTION(): Automatically uses the function name as the event name and records the total execution time of the function.
* YTRACING_SCOPE("Name"): Uses a custom name to record the execution time of a specific code block, suitable for localized performance analysis inside functions.

---

## Integration into other projects

1. Install `libytracing-dev` (see Installation section)
2. Add the linking in your `CMakeLists.txt`:

```cmake
find_package(ytracing REQUIRED)
target_link_libraries(your_target PRIVATE ytracing)
```

---

## License

MIT License
