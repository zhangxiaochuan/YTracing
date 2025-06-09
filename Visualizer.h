#ifndef YTRACING_VISUALIZER_H
#define YTRACING_VISUALIZER_H

#include <string>

namespace YTracing {

class Visualizer {
public:
    static Visualizer& instance();
    
    // 生成完整的HTML页面
    std::string generate_html(const std::string& json_data);
    
    // 保存HTML文件
    void save_to_file(const std::string& html, const std::string& filename);

private:
    Visualizer() = default;
    
    std::string get_html_template();
    std::string get_js_code();
};

} // namespace YTracing

#endif // YTRACING_VISUALIZER_H
