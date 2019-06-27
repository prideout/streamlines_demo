#include "demo.h"

extern app_state app;

void init_demo_simple(app_state* app, int variant_index);
void draw_demo_simple(app_state* app, int variant_index);

void init_demo_wireframe(app_state* app, int variant_index);
void draw_demo_wireframe(app_state* app, int variant_index);

void init_demo_gradient(app_state* app, int variant_index);
void draw_demo_gradient(app_state* app, int variant_index);

void init_demo_closed(app_state* app, int variant_index);
void draw_demo_closed(app_state* app, int variant_index);

void init_demo_endcap(app_state* app, int variant_index);
void draw_demo_endcap(app_state* app, int variant_index);

void init_demo_noisy(app_state* app, int variant_index);
void draw_demo_noisy(app_state* app, int variant_index);

void init_demo_streamlines(app_state* app, int variant_index);
void draw_demo_streamlines(app_state* app, int variant_index);

void init_common(demo_type current_demo, int current_variant) {
    app.variants[current_variant].pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };
    switch (current_demo) {
        case DEMO_SIMPLE: init_demo_simple(&app, current_variant); break;
        case DEMO_WIREFRAME: init_demo_wireframe(&app, current_variant); break;
        case DEMO_GRADIENT: init_demo_gradient(&app, current_variant); break;
        case DEMO_CLOSED: init_demo_closed(&app, current_variant); break;
        case DEMO_ENDCAP: init_demo_endcap(&app, current_variant); break;
        case DEMO_NOISY: init_demo_noisy(&app, current_variant); break;
        case DEMO_STREAMLINES: init_demo_streamlines(&app, current_variant); break;
    }
}

void draw_common(demo_type current_demo, int current_variant) {
    switch (current_demo) {
        case DEMO_SIMPLE: draw_demo_simple(&app, current_variant); break;
        case DEMO_WIREFRAME: draw_demo_wireframe(&app, current_variant); break;
        case DEMO_GRADIENT: draw_demo_gradient(&app, current_variant); break;
        case DEMO_CLOSED: draw_demo_closed(&app, current_variant); break;
        case DEMO_ENDCAP: draw_demo_endcap(&app, current_variant); break;
        case DEMO_NOISY: draw_demo_noisy(&app, current_variant); break;
        case DEMO_STREAMLINES: draw_demo_streamlines(&app, current_variant); break;
    }
}
