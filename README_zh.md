[English](README.md) | [中文](README_zh.md)
## YTracing

### 项目概述

**YTracing** 是一个使用 C++17 实现的轻量级、仅头文件依赖的C++事件跟踪库。它通过 `AutoTracer` 的 RAII (资源获取即初始化) 机制，自动记录函数调用或代码块的开始与结束时间。`Collector` 单例在后台线程运行，定期将跟踪事件写入带时间戳的目录中的 `.raw` 原始文件。

最后，可以使用 `YViewer` 命令行工具将这些原始跟踪文件转换为 Perfetto 兼容的 JSON 格式，并加载到 [Perfetto UI](https://ui.perfetto.dev) 等可视化工具中进行深入分析。

### 项目特性

  * **轻量级**: 性能开销极小，适用于对性能敏感的应用。
  * **仅头文件依赖**: 开始跟踪时，你只需要包含 `YTracing.h` 头文件。
  * **基于 RAII 的自动跟踪**: 只需在函数或作用域中添加一个宏，即可自动跟踪其生命周期。
  * **带时间戳的目录**: 每次运行都会创建一个唯一的、带时间戳的目录 (例如 `tracing_20250810_224500`) 来存储跟踪日志，防止覆盖上一次运行的数据。
  * **Perfetto 兼容**: 生成的 JSON 文件可直接在 Perfetto 网页版 UI 中进行可视化。

### 项目结构

项目采用清晰、模块化的目录结构：

```
YTracing/
├── CMakeLists.txt          # 根目录 CMake 文件
├── src/                    # 核心库源代码 (YTracingCore, YTracingVisualizer)
├── examples/               # 展示如何使用库的示例程序
└── tools/                    # 命令行工具，如 YViewer
```

### 依赖项

  * **CMake** (3.10 或更高版本)
  * 兼容 C++17 的编译器 (例如 GCC, Clang)
  * **平台**: Linux / macOS (依赖 `pthread` 等 POSIX API)

### 编译指南

1.  克隆仓库并创建构建目录：

    ```bash
    git clone https://your-repository-url/YTracing.git
    cd YTracing
    mkdir build
    cd build
    ```

2.  运行 CMake 并构建项目：

    ```bash
    cmake ..
    make
    ```

3.  构建成功后，将生成以下可执行文件：

      * `build/examples/test`: 一个用于生成跟踪文件的示例程序。
      * `build/tools/YViewer`: 用于将原始跟踪文件转换为 JSON 的工具。

### 使用说明

1.  **埋点你的代码**: 在你希望跟踪的代码中，包含 `YTracing.h` 头文件并使用预定义的宏。

      * `YTRACING_FUNCTION()`: 跟踪它所在的整个函数作用域。
      * `YTRACING_SCOPE("My Custom Scope")`: 跟踪一个自定义命名的作用域。

    <!-- end list -->

    ```cpp
    #include "YTracing.h"
    #include <thread>

    void my_function() {
        YTRACING_FUNCTION(); // 跟踪此函数
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    int main() {
        YTRACING_SCOPE("main_thread"); // 跟踪主线程作用域
        my_function();
        return 0;
    }
    ```

2.  **生成跟踪文件**: 编译并运行你的程序 (或仓库中提供的 `test` 示例)。程序将在其运行目录下创建一个名为 `tracing_YYYYMMDD_HHMMSS/` 的新目录，其中包含 `.raw` 格式的原始跟踪文件。

3.  **转换为 JSON**: 运行 `YViewer` 工具，并将跟踪目录的路径作为参数传入。

    ```bash
    # 假设你的可执行文件在 'build' 目录，且它创建了一个跟踪目录
    cd build/examples
    ./test  # 这将在此处创建一个 'tracing_...' 目录

    # 现在，从 build 目录运行 YViewer
    cd ../tools
    ./YViewer ../examples/tracing_.../ # 此处传入实际的目录名
    ```

    这将在当前目录 (`build/tools/`) 下生成一个 `trace.json` 文件。

4.  **可视化**: 在浏览器中打开 [Perfetto UI](https://ui.perfetto.dev)，点击 "Open trace file"，然后选择你刚刚创建的 `trace.json` 文件。

### 模块结构图

```
+-------------+        +-----------+
| AutoTracer  | -----> | Collector |
+-------------+        +-----------+
                              |
                              v
                   +----------------------+
                   |   带时间戳的日志目录   |
                   | (trace_....raw 文件) |
                   +----------------------+
                              |
                              v
                        +-----------+
                        |  YViewer  | (使用 Converter)
                        +-----------+
                              |
                              v
                        +-------------+
                        | trace.json  |
                        +-------------+
```