-- wireframe.vs

uniform vec4 resolution;
layout(location=0) in vec2 position;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
}

-- gradient.vs

uniform vec4 resolution;
uniform float apply_gradient;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
out vec4 vannotation;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
  vannotation = annotation;
}

-- closed.vs

uniform vec4 resolution;
layout(location=0) in vec2 position;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  gl_Position = vec4(p, 0.0, 1.0);
}

-- endcap.vs

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
}

-- noisy.vs

uniform vec4 resolution;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
uniform float variant;

// <https:--www.shadertoy.com/view/4dS3Wd>
// By Morgan McGuire @morgan3d, http:--graphicscodex.com
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
  if (variant < 0.5) {
      float wave = 0.5 + 0.5 * sin(10.0 * 6.28318 * annotation.x);
      p += spine_to_edge * 0.01 * wave;
  }
  p += annotation.y * spine_to_edge * 0.005 * noise(100.0 * sin(6.28 * annotation.x));
  gl_Position = vec4(p, 0.0, 1.0);
}

-- streamlines.vs

uniform vec4 resolution;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
layout(location=2) in float offset;
out vec4 vannotation;
out float voffset;
void main() {
    vec2 p = 2.0 * position * resolution.xy - 1.0;
    gl_Position = vec4(p, 0.0, 1.0);
    vannotation = annotation;
    voffset = offset;
}

-- curves.vs

uniform vec4 resolution;
uniform float apply_gradient;
layout(location=0) in vec2 position;
layout(location=1) in vec4 annotation;
out vec4 vannotation;
void main() {
  vec2 p = 2.0 * position * resolution.xy - 1.0;
  p = p * vec2(2.0, 1.8) + vec2(1.25, 0.85); // hack to adjust the viewport
  gl_Position = vec4(p, 0.0, 1.0);
  vannotation = annotation;
}

-- wireframe.fs 

out vec4 frag_color;
void main() {
  frag_color = vec4(0, 0, 0, 1);
}

-- gradient.fs 

uniform vec4 resolution;
uniform float apply_gradient;
in vec4 vannotation;
out vec4 frag_color;
void main() {
  float t = vannotation.x;
  vec3 color = mix(vec3(.0, .0, .8), vec3(.0, .8, .0), t);
  frag_color = vec4(apply_gradient == 0.0 ? vec3(0) : color, 1);
}

-- closed.fs 

out vec4 frag_color;
void main() {
  frag_color = vec4(0.0, 0.0, 0.0, 0.8);
}

-- endcap.fs 

in vec4 varying_annotation;
in float varying_length;
out vec4 frag_color;
uniform float variant;
const float radius = 15.0;
const float radius2 = radius * radius;
void main() {
  float dist1 = abs(varying_annotation.x);
  float dist2 = varying_length - abs(varying_annotation.x);
  float dist = variant > 0.5 ? min(dist1, dist2) : dist1;
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
}

-- noisy.fs 

out vec4 frag_color;
void main() {
  frag_color = vec4(0.0, 0.0, 0.0, 0.8);
}

-- streamlines.fs

uniform float time;
uniform float variant;
in float voffset;
in vec4 vannotation;
out vec4 frag_color;
void main() {
  float L = vannotation.x - time;
  if (variant >= 0.5) {
      L += voffset;
  }
  frag_color = vec4(0.0, 0.0, 0.0, 0.5 * fract(L));
}

-- curves.fs 

uniform vec4 resolution;
in vec4 vannotation;
out vec4 frag_color;
void main() {
  float t = vannotation.x;
  frag_color = vec4(0.0, 0.0, 0.0, 0.75);
}
