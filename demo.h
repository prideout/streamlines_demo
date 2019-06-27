#pragma once

#include "sokol_gfx.h"

#include <par/par_streamlines.h>

#define PI 3.14159265358979323846

typedef enum {
    DEMO_SIMPLE = 0,
    DEMO_WIREFRAME = 1,
    DEMO_GRADIENT = 2,
    DEMO_CLOSED = 3,
    DEMO_ENDCAP = 4, // <-- this has variations
    DEMO_NOISY = 5,  // <-- this has variations
    DEMO_STREAMLINES = 6,  // <-- this has variations
    // TODO: cubic and quadratic curves
} demo_type;

#define DEMO_TYPE_COUNT 7

#define VARIANT_COUNT 7

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
} variant_state;

typedef struct {
    uint64_t start_time;
    variant_state variants[VARIANT_COUNT];
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

void init_common(demo_type current_demo, int current_variant);
void draw_common(demo_type current_demo, int current_variant);

const char* get_vertex_shader(demo_type);
const char* get_fragment_shader(demo_type);
