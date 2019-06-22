#pragma once

#include "sokol_gfx.h"

#include <par/par_streamlines.h>

typedef enum {
    DEMO_SIMPLE = 0,
    DEMO_WIREFRAME = 1,
    DEMO_GRADIENT = 2,
    DEMO_CLOSED_SPINE = 3,
    DEMO_COLOR_PER_SPINE = 4,
    DEMO_COLOR_PER_SEGMENT = 5,
    DEMO_ENDCAP_SHADER = 6,
    DEMO_NOISY_LINES = 7,
    DEMO_STREAMLINES = 8,
} demo_type;

#define DEMO_COUNT 9

typedef struct {
    par_streamlines_context* context;
    sg_pipeline pipeline;
    sg_bindings bindings;
    sg_buffer positions_buffer;
    sg_buffer annotations_buffer;
    int num_elements;
    par_streamlines_spine_list spines;
} demo_state;

typedef struct {
    sg_pass_action pass_action;
    uint64_t start_time;
    demo_state demos[DEMO_COUNT];
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
