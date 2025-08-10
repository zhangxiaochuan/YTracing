[English](README.md) | [中文](README_zh.md)

## YTracing
### Project Overview

**YTracing** is a lightweight, header-only C++ event tracing library implemented in C++17. It leverages the RAII (Resource Acquisition Is Initialization) mechanism through `AutoTracer` to automatically record the start and end times of function calls or scopes. The `Collector` singleton runs in a background thread, periodically writing trace events to `.raw` files in a timestamped directory.

Finally, the `YViewer` command-line tool can be used to convert these raw trace files into a Perfetto-compatible JSON format, which can be loaded into visualization tools like the [Perfetto UI](https://ui.perfetto.dev) for further analysis.

### Features

  * **Lightweight**: Minimal performance overhead, suitable for performance-critical applications.
  * **Header-Only Usage**: To start tracing, you only need to include `YTracing.h`.
  * **RAII-based Auto-Tracing**: Simply add a macro to a function or scope to automatically trace its lifetime.
  * **Timestamped Directories**: Each run creates a unique, timestamped directory (e.g., `tracing_20250810_224500`) to store traces, preventing data from previous runs from being overwritten.
  * **Perfetto Compatible**: Generates JSON files that can be directly visualized in the Perfetto web UI.

### Project Structure

The project is organized into a clean, modular structure:

```
YTracing/
├── CMakeLists.txt          # Root CMake file
├── src/                    # Core library source code (YTracingCore, YTracingVisualizer)
├── examples/               # Example program showing how to use the library
└── tools/                    # Command-line tools like YViewer
```

### Dependencies

  * **CMake** (version 3.10 or higher)
  * A C++17 compatible compiler (e.g., GCC, Clang)
  * **Platform**: Linux / macOS (relies on POSIX APIs like `pthread`)

### Build Instructions

1.  Clone the repository and create a build directory:

    ```bash
    git clone https://your-repository-url/YTracing.git
    cd YTracing
    mkdir build
    cd build
    ```

2.  Run CMake and build the project:

    ```bash
    cmake ..
    make
    ```

3.  After a successful build, the following executables will be generated:

      * `build/examples/test`: An example program that generates trace files.
      * `build/tools/YViewer`: The tool used to convert raw traces to JSON.

### Usage Guide

1.  **Instrument Your Code**: In the code you want to trace, include the `YTracing.h` header and use the provided macros.

      * `YTRACING_FUNCTION()`: Traces the entire scope of the function it's placed in.
      * `YTRACING_SCOPE("My Custom Scope")`: Traces a specific, named scope.

    <!-- end list -->

    ```cpp
    #include "YTracing.h"
    #include <thread>

    void my_function() {
        YTRACING_FUNCTION(); // Traces this function
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    int main() {
        YTRACING_SCOPE("main_thread"); // Traces the main scope
        my_function();
        return 0;
    }
    ```

2.  **Generate Trace Files**: Compile and run your program (or the provided `test` example). It will create a new directory in the location where it was run, named `tracing_YYYYMMDD_HHMMSS/`, containing the raw `.raw` trace files.

3.  **Convert to JSON**: Run the `YViewer` tool, passing the path to the trace directory as an argument.

    ```bash
    # Assuming your executable is in 'build' and it created a trace dir
    cd build/examples
    ./test  # This will create a 'tracing_...' directory here

    # Now, run YViewer from the build directory
    cd ../tools
    ./YViewer ../examples/tracing_.../ # Pass the actual directory name
    ```

    This will generate a `trace.json` file in the current directory (`build/tools/`).

4.  **Visualize**: Open the [Perfetto UI](https://ui.perfetto.dev) in your browser, click "Open trace file", and select the `trace.json` you just created.

### Module Diagram

```
+-------------+        +-----------+
| AutoTracer  | -----> | Collector |
+-------------+        +-----------+
                              |
                              v
                   +----------------------+
                   | Timestamped Log Dir  |
                   | (trace_....raw files)|
                   +----------------------+
                              |
                              v
                        +-----------+
                        |  YViewer  | (uses Converter)
                        +-----------+
                              |
                              v
                        +-------------+
                        | trace.json  |
                        +-------------+
```

