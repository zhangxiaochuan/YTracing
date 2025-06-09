# Contribution Guidelines

* All C++ functions in this repository must invoke `YTRACING_FUNCTION()` at the very beginning of the function body so profiling is enabled.
* After any code change you must compile the project using:
  ```
  cmake -S . -B build
  cmake --build build -j$(nproc)
  ```
  Ensure the build completes successfully. Documentation or comment-only updates do not require running this build step.
