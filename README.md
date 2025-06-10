# YTracing

## 项目概述
YTracing 是一个使用 C++17 实现的轻量级事件跟踪库。通过 `AutoTracer` 的 RAII 机制，库在代码运行时自动记录事件的开始和结束时间，并由 `Collector` 在后台线程定期将事件写入 `tracing/` 目录中的原始文件。随后可利用 `Converter` 将数据转换为 Perfetto 兼容的 JSON 格式，供其他工具进一步分析。

## 模块结构图
```
+-------------+        +-----------+
| AutoTracer  | -----> | Collector |
+-------------+        +-----------+
                              |
                              v
                        +------------+
                        | Trace Files|
                        +------------+
                              |
                              v
                        +-----------+
                        | Converter |
                        +-----------+
```
- **AutoTracer**：在构造和析构时记录事件。可通过 `YTRACING_FUNCTION()` 或 `YTRACING_SCOPE()` 宏使用。
- **Collector**：单例类，负责收集各线程事件并定期刷写到磁盘。
- **Converter**：读取原始 trace 文件并导出 Perfetto JSON。
- **main**：示例程序，演示如何产生 trace 数据。
- **YViewer**：命令行工具，将 trace 目录转换为 JSON 文件。

## 支持平台
- Linux / macOS（需要支持 C++17 与 POSIX API 的编译器）

## 编译方式
```bash
mkdir build
cd build
cmake ..
make
```
编译完成后将生成：
- `main`：示例程序，运行后在 `tracing/` 目录生成原始 trace 文件。
- `YViewer`：转换与可视化工具。

## 使用说明
1. 在需要跟踪的函数或代码块中使用 `YTRACING_FUNCTION()` 或 `YTRACING_SCOPE("名称")` 宏。
2. 编译并运行你的程序（或仓库中的 `main`），执行结束后会在 `tracing/` 目录得到 `trace_<pid>_<tid>.raw` 文件。
3. 运行 `./YViewer tracing`（传入原始文件所在目录），工具会生成 `trace.json`。
