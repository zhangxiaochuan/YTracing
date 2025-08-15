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


---

## 许可证

MIT License

