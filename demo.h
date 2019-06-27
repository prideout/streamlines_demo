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

#define DEMO_COUNT 7

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
} demo_state;

typedef struct {
    uint64_t start_time;
    demo_state demos[DEMO_COUNT];
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

void init_demo_simple(app_state* app);
void draw_demo_simple(app_state* app);

void init_demo_wireframe(app_state* app);
void draw_demo_wireframe(app_state* app);

void init_demo_gradient(app_state* app);
void draw_demo_gradient(app_state* app);

void init_demo_closed(app_state* app);
void draw_demo_closed(app_state* app);

void init_demo_endcap(app_state* app);
void draw_demo_endcap(app_state* app);

void init_demo_noisy(app_state* app);
void draw_demo_noisy(app_state* app);

void init_demo_streamlines(app_state* app);
void draw_demo_streamlines(app_state* app);

const char* get_vertex_shader(demo_type);
const char* get_fragment_shader(demo_type);
