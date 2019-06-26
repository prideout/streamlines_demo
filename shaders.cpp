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
void main() {
    vec2 p = 2.0 * position * resolution.xy - 1.0;
    gl_Position = vec4(p, 0.0, 1.0);
})",

// Wireframe Vertex Shader
PREAMBLE R"(
uniform vec4 resolution;
layout(location=0) in vec2 position;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
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

// Closed Vertex Shader
PREAMBLE R"(
uniform vec4 resolution;
layout(location=0) in vec2 position;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
})",

// Endcap Vertex Shader
PREAMBLE R"(
uniform vec4 resolution;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
layout(location=2) in float length;
out vec4 varying_annotation;
out float varying_length;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
  varying_annotation = annotation;
  varying_length = length;
})",

// Noisy Vertex Shader
PREAMBLE R"(
uniform vec4 resolution;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;

// <https://www.shadertoy.com/view/4dS3Wd>
// By Morgan McGuire @morgan3d, http://graphicscodex.com
// 
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
float noise(float x) {
  float i = floor(x);
  float f = fract(x);
  float u = f * f * (3.0 - 2.0 * f);
  return mix(hash(i), hash(i + 1.0), u);
}

void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  vec2 spine_to_edge = annotation.zw;
//  float wave = 0.5 + 0.5 * sin(10.0 * 6.28318 * annotation.x);
//  p += spine_to_edge * 0.01 * wave;
  p += annotation.y * spine_to_edge * 0.005 * noise(100.0 * sin(6.28 * annotation.x));
  gl_Position = vec4(p, 0.0, 1.0);
})",

// Streamlines Vertex Shader
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
out vec4 frag_color;
void main() {
    frag_color = vec4(0, 0, 0, 1);
})",

// Wireframe Fragment Shader
PREAMBLE R"(
precision highp float;
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

// Closed Fragment Shader
PREAMBLE R"(
precision highp float;
out vec4 frag_color;
void main() {
  frag_color = vec4(0.0, 0.0, 0.0, 0.8);
})",

// Endcap Fragment Shader
PREAMBLE R"(
precision highp float;
in vec4 varying_annotation;
in float varying_length;
out vec4 frag_color;
const float radius = 15.0;
const float radius2 = radius * radius;
void main() {
  float dist1 = abs(varying_annotation.x);
  float dist2 = varying_length - abs(varying_annotation.x);
  float dist = min(dist1, dist2);
  float alpha = 1.0;
  if (dist < radius) {
      float x = dist - radius;
      float y = varying_annotation.y * radius;
      float d2 = x * x + y * y;
      float t = fwidth(d2);
      alpha = 1.0 - 0.99 * smoothstep(radius2 - t, radius2 + t, d2);
      // alpha = 1.0 - 0.99 * step(radius2, d2);
  }
  frag_color = vec4(0, 0, 0, alpha);
})",

// Noisy Fragment Shader
PREAMBLE R"(
precision highp float;
out vec4 frag_color;
void main() {
  frag_color = vec4(0.0, 0.0, 0.0, 0.8);
})",

// Streamlines Fragment Shader
PREAMBLE R"(
precision highp float;
in vec4 vannotation;
out vec4 frag_color;
void main() {
  frag_color = vec4(0, 0, 0, 1);
})",

};

const char* get_vertex_shader(demo_type demo) {
    return vs[demo];
}

const char* get_fragment_shader(demo_type demo) {
    return fs[demo];
}

#include <par/par_streamlines.h>

__attribute__((unused))
static void test_cpp() {
    parsl_context* ctx = parsl_create_context({ .thickness = 3 });
    parsl_position vertices[] = { {0, 0}, {2, 1}, {4, 0} };
    uint16_t spine_lengths[] = { 3 };
    parsl_mesh_from_lines(ctx, {
        .num_vertices = sizeof(vertices) / sizeof(parsl_position),
        .num_spines = sizeof(spine_lengths) / sizeof(uint16_t),
        .vertices = vertices,
        .spine_lengths = spine_lengths
    });
    parsl_destroy_context(ctx);
}
