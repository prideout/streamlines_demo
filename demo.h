#pragma once

#include "sokol_gfx.h"

#include <par/par_streamlines.h>

#define PI 3.14159265358979323846

typedef enum {
    DEMO_WIREFRAME = 0,
    DEMO_GRADIENT = 1,
    DEMO_CLOSED = 2,
    DEMO_ENDCAP = 3,
    DEMO_NOISY = 4,
    DEMO_STREAMLINES = 5,
    // TODO: cubic and quadratic curves
} demo_type;

#define DEMO_TYPE_COUNT 6
#define CANVAS_COUNT 10

typedef struct {
    parsl_context* context;
    sg_pass_action pass_action;
    sg_pipeline pipeline;
    sg_bindings bindings;
    sg_buffer positions_buffer;
    sg_buffer annotations_buffer;
    sg_buffer lengths_buffer;
    sg_buffer offsets_buffer;
    int num_elements;
    parsl_spine_list spines;
    int em_context;
    sg_context gfx_context;
    demo_type demo;
    int demo_variant;
} canvas_state;

typedef struct {
    uint64_t start_time;
    canvas_state canvases[CANVAS_COUNT];
    int width;
    int height;
    float pixel_ratio;
} app_state;

typedef struct {
    float inverse_width;
    float inverse_height;
    float width;
    float height;
} uniform_params;

void init_common(demo_type demo_index, int canvas_index);
void draw_common(int canvas_index);

const char* get_vertex_shader(demo_type);
const char* get_fragment_shader(demo_type);
