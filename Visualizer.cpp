#include "Visualizer.h"
#include <fstream>

namespace YTracing {

Visualizer& Visualizer::instance() {
    static Visualizer visualizer;
    return visualizer;
}

std::string Visualizer::generate_html(const std::string& json_data) {
    std::string html = get_html_template();
    std::string js = get_js_code();
    
    // 替换模板中的占位符
    size_t js_pos = html.find("/*{{JS_CODE}}*/");
    if (js_pos != std::string::npos) {
        html.replace(js_pos, 15, js);
    }
    
    size_t data_pos = html.find("/*{{TRACE_DATA}}*/");
    if (data_pos != std::string::npos) {
        html.replace(data_pos, 18, json_data);
    }
    
    return html;
}

void Visualizer::save_to_file(const std::string& html, const std::string& filename) {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << html;
    }
}

std::string Visualizer::get_html_template() {
    return R"(
<!DOCTYPE html>
<html>
<head>
  <title>YTracing Visualization</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 20px; }
    #timeline { width: 100%; height: 600px; }
    .event { cursor: pointer; }
    .tooltip {
      position: absolute;
      background: rgba(0, 0, 0, 0.8);
      color: #fff;
      padding: 5px;
      border-radius: 3px;
      pointer-events: none;
      font-size: 12px;
    }
  </style>
  <script src="https://d3js.org/d3.v7.min.js"></script>
  <script>
    /*{{JS_CODE}}*/
  </script>
</head>
<body>
  <h1>YTracing Timeline</h1>
  <div id="timeline"></div>
</body>
</html>
)";
}

std::string Visualizer::get_js_code() {
    return R"JS_CODE(
// Set up dimensions and scales
var margin = {top: 20, right: 30, bottom: 30, left: 150};
var width = 1200 - margin.left - margin.right;
var height = 600 - margin.top - margin.bottom;

var x = d3.scaleLinear().range([0, width]);
var y = d3.scaleBand().range([height, 0]).padding(0.1);

// Create SVG container
var svg = d3.select("#timeline")
  .append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

// Add tooltip
var tooltip = d3.select("body").append("div")
  .attr("class", "tooltip")
  .style("opacity", 0);

// Parse trace data
var traceData = /*{{TRACE_DATA}}*/;

// Process data
var events = traceData.filter(function(d) {
  return (d.ph === 'B' || d.ph === 'E') && d.dur;
}).map(function(d) {
  return {
    thread: 'Thread ' + d.tid,
    name: d.name,
    start: d.ts / 1000,
    end: (d.ts + d.dur) / 1000,
    duration: d.dur / 1000,
    args: d.args || {}
  };
});

// Set domains
x.domain([d3.min(events, function(d) { return d.start; }), 
          d3.max(events, function(d) { return d.end; })]);

y.domain(events.map(function(d) { return d.thread; }));

// Add X axis
svg.append("g")
  .attr("transform", "translate(0," + height + ")")
  .call(d3.axisBottom(x).ticks(10).tickFormat(d3.format(".2f")));

// Add Y axis
svg.append("g")
  .call(d3.axisLeft(y));

// Add event rectangles
svg.selectAll(".event")
  .data(events)
  .enter().append("rect")
    .attr("class", "event")
    .attr("x", function(d) { return x(d.start); })
    .attr("y", function(d) { return y(d.thread); })
    .attr("width", function(d) { return x(d.end) - x(d.start); })
    .attr("height", y.bandwidth())
    .attr("fill", "#4CAF50")
    .on("mouseover", function(event, d) {
      tooltip.transition()
        .duration(200)
        .style("opacity", .9);
      tooltip.html("Event: " + d.name + "<br/>" +
                  "Duration: " + d.duration.toFixed(2) + " μs<br/>" +
                  "Thread: " + d.thread)
        .style("left", (event.pageX + 5) + "px")
        .style("top", (event.pageY - 28) + "px");
    })
    .on("mouseout", function(d) {
      tooltip.transition()
        .duration(500)
        .style("opacity", 0);
    });

// Add zoom behavior
var zoom = d3.zoom()
  .scaleExtent([1, 10])
  .on("zoom", zoomed);

svg.call(zoom);

function zoomed(event) {
  var newX = event.transform.rescaleX(x);
  svg.selectAll(".event")
    .attr("x", function(d) { return newX(d.start); })
    .attr("width", function(d) { return newX(d.end) - newX(d.start); });
  svg.select(".x.axis").call(d3.axisBottom(newX));
}
)JS_CODE";
}

} // namespace YTracing
