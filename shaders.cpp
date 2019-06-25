extern "C" {
    #include "demo.h"
}

#ifdef EMSCRIPTEN
#define PREAMBLE "#version 300 es\n"
#else
#define PREAMBLE "#version 330\n"
#endif

static const char* vs[DEMO_COUNT] = {

// Simple Vertex Shader
PREAMBLE R"(
uniform vec4 resolution;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
out vec4 vannotation;
void main() {
    vec2 p = 2.0 * position * resolution.xy - 1.0;
    gl_Position = vec4(p, 0.0, 1.0);
    vannotation = annotation;
})",

// Wireframe Vertex Shader
PREAMBLE R"(
uniform vec4 resolution;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
out vec4 vannotation;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
  vannotation = annotation;
})",

// Gradient Vertex Shader
PREAMBLE R"(
uniform vec4 resolution;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
out vec4 vannotation;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
  vannotation = annotation;
})",

};

static const char* fs[DEMO_COUNT] = {

// Simple Fragment Shader
PREAMBLE R"(
precision highp float;
in vec4 vannotation;
out vec4 frag_color;
void main() {
    frag_color = vec4(0, 0, 0, 1);
})",

// Wireframe Fragment Shader
PREAMBLE R"(
precision highp float;
in vec4 vannotation;
out vec4 frag_color;
void main() {
  frag_color = vec4(0, 0, 0, 1);
})",

// Gradient Fragment Shader
PREAMBLE R"(
precision highp float;
in vec4 vannotation;
out vec4 frag_color;
void main() {
  float t = vannotation.x;
  vec3 color = mix(vec3(.0, .0, .8), vec3(.0, .8, .0), t);
  frag_color = vec4(color, 1);
})",

};

const char* get_vertex_shader(demo_type demo) {
    return vs[demo];
}

const char* get_fragment_shader(demo_type demo) {
    return fs[demo];
}
