#include "demo.h"

extern app_state app;

void init_demo_simple(app_state* app, int canvas_index);
void draw_demo_simple(app_state* app, int canvas_index);

void init_demo_wireframe(app_state* app, int canvas_index);
void draw_demo_wireframe(app_state* app, int canvas_index);

void init_demo_gradient(app_state* app, int canvas_index);
void draw_demo_gradient(app_state* app, int canvas_index);

void init_demo_closed(app_state* app, int canvas_index);
void draw_demo_closed(app_state* app, int canvas_index);

void init_demo_endcap(app_state* app, int canvas_index);
void draw_demo_endcap(app_state* app, int canvas_index);

void init_demo_noisy(app_state* app, int canvas_index);
void draw_demo_noisy(app_state* app, int canvas_index);

void init_demo_streamlines(app_state* app, int canvas_index);
void draw_demo_streamlines(app_state* app, int canvas_index);

void init_common(demo_type demo_index, int canvas_index) {
    app.canvases[canvas_index].demo = demo_index;
    app.canvases[canvas_index].pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };
    switch (demo_index) {
        case DEMO_SIMPLE: init_demo_simple(&app, canvas_index); break;
        case DEMO_WIREFRAME: init_demo_wireframe(&app, canvas_index); break;
        case DEMO_GRADIENT: init_demo_gradient(&app, canvas_index); break;
        case DEMO_CLOSED: init_demo_closed(&app, canvas_index); break;
        case DEMO_ENDCAP: init_demo_endcap(&app, canvas_index); break;
        case DEMO_NOISY: init_demo_noisy(&app, canvas_index); break;
        case DEMO_STREAMLINES: init_demo_streamlines(&app, canvas_index); break;
    }
}

void draw_common(int canvas_index) {
    switch (app.canvases[canvas_index].demo) {
        case DEMO_SIMPLE: draw_demo_simple(&app, canvas_index); break;
        case DEMO_WIREFRAME: draw_demo_wireframe(&app, canvas_index); break;
        case DEMO_GRADIENT: draw_demo_gradient(&app, canvas_index); break;
        case DEMO_CLOSED: draw_demo_closed(&app, canvas_index); break;
        case DEMO_ENDCAP: draw_demo_endcap(&app, canvas_index); break;
        case DEMO_NOISY: draw_demo_noisy(&app, canvas_index); break;
        case DEMO_STREAMLINES: draw_demo_streamlines(&app, canvas_index); break;
    }
}
