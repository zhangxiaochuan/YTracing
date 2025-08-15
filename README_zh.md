[English](README.md) | [中文](README_zh.md)
# YTracing

YTracing 是一个高性能的C++事件追踪库，支持多线程、多进程环境下的性能分析与可视化。

---

## 特性

- 轻量级、低开销的性能追踪  
- 支持多线程、多进程事件记录  
- 生成兼容 [Perfetto](https://perfetto.dev/) 的可视化数据  
- 提供命令行工具进行数据处理与分析  

---

## 安装

### 方式一：通过 apt 安装

1. 添加 PPA 仓库：
```bash
sudo add-apt-repository ppa:zhangxiaochuan/ytracing
sudo apt update
````

2. 安装开发包（包含头文件和库文件）：

```bash
sudo apt install libytracing-dev
```

### 方式二：通过源码编译

1. 克隆项目：

```bash
git clone https://github.com/zhangxiaochuan/YTracing.git
cd YTracing
```

2. 编译并安装：

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

---

## 使用

### 启用追踪功能


在代码中引入头文件，并使用以下两种方法记录追踪信息：

```cpp
#include <YTracing/YTracing.h>

void my_function() {
    // 方法一：YTRACING_FUNCTION
    // 记录整个函数的执行时间（作用域是整个函数）
    YTRACING_FUNCTION();

    // 模拟代码执行
    for (int i = 0; i < 1000000; ++i) {}

    {
        // 方法二：YTRACING_SCOPE("Custom Scope Name")
        // 记录一个自定义代码块的执行时间（作用域仅限于花括号内）
        YTRACING_SCOPE("Inner Loop Work");

        for (int j = 0; j < 500000; ++j) {}
    }
}
```
区别说明：

* YTRACING_FUNCTION()：自动以函数名作为事件名称，追踪整个函数的执行耗时。
* YTRACING_SCOPE("Name")：以自定义名称追踪特定代码块的执行耗时，适合函数内部的局部性能分析。


在 `CMakeLists.txt` 中添加以下定义以启用追踪，并链接YTracing库：

```cmake
add_definitions(-DTRACING=1) # 启用追踪功能，去掉这行即可关闭追踪功能

find_package(ytracing REQUIRED)
target_link_libraries(your_target PRIVATE YTracing::YTracingCore)
```

### 使用 **YViewer** 进行追踪可视化

为方便对跟踪数据进行直观分析，**YTracing** 提供了 `YViewer` 命令行工具，帮助你可视化跟踪结果：

1. 完成编译之后，可以执行目标程序，并在工作目录下生成tracing_YYYYMMDD_HHMMSS文件夹，其中包括`.raw`格式的原始追踪数据。
2. 使用YViewer**将原始 `.raw` 文件转为 Perfetto 格式**  
   `YViewer` 会将由 YTracing 生成的 `.raw` 跟踪数据转换为可兼容 Perfetto 的 `trace.json` 格式。

3. **在 Perfetto UI 中查看与分析**  
   在浏览器中打开 [Perfetto Web UI](https://ui.perfetto.dev)，加载生成的 `trace.json` 后，你可以：
    - 查看函数调用与作用域嵌套关系及其耗时情况；
    - 通过缩放、平移、名称或时间过滤等操作，精准定位感兴趣的部分；
    - 在可视化时间线界面中分析并发执行、线程活动与函数执行时序等。

4. **推荐使用流程**

   ```bash
   # 1. 运行已插桩的程序：
   ./your_target
   # 此操作会生成一个包含 .raw 文件的 tracing_YYYYMMDD_HHMMSS 目录。

   # 2. 将跟踪数据转换为 JSON：
   YViewer tracing_YYYYMMDD_HHMMSS/
   # 将在tracing_YYYYMMDD_HHMMSS目录下将生成 trace.json 文件。

   # 3. 打开并可视化：
   浏览器打开 [Perfetto UI](https://ui.perfetto.dev) → 点击 “Open trace file” → 选择 `trace.json`。


---

## 许可证

MIT License

