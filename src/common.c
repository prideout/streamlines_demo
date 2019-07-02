#include "demo.h"
#include "shaders.h"

#include <string.h>

extern app_state app;

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

void init_demo_curves(app_state* app, int canvas_index);
void draw_demo_curves(app_state* app, int canvas_index);

void init_common(demo_type demo_index, int canvas_index) {

    app.shaders = parsh_create_context((parsh_config) {});
    parsh_add_blocks(app.shaders, SHADERS, strlen(SHADERS));

#ifdef EMSCRIPTEN
    parsh_add_block(app.shaders, "vsprefix", "#version 300 es\n");
    parsh_add_block(app.shaders, "fsprefix", "#version 300 es\n"
        "precision highp float;\n");
#else
    parsh_add_block(app.shaders, "vsprefix", "#version 330\n");
    parsh_add_block(app.shaders, "fsprefix", "#version 330\n"
        "precision highp float;\n");
#endif

    app.canvases[canvas_index].demo = demo_index;
    app.canvases[canvas_index].pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };
    switch (demo_index) {
        case DEMO_WIREFRAME: init_demo_wireframe(&app, canvas_index); break;
        case DEMO_GRADIENT: init_demo_gradient(&app, canvas_index); break;
        case DEMO_CLOSED: init_demo_closed(&app, canvas_index); break;
        case DEMO_ENDCAP: init_demo_endcap(&app, canvas_index); break;
        case DEMO_NOISY: init_demo_noisy(&app, canvas_index); break;
        case DEMO_STREAMLINES: init_demo_streamlines(&app, canvas_index); break;
        case DEMO_CURVES: init_demo_curves(&app, canvas_index); break;
    }
}

void draw_common(int canvas_index) {
    switch (app.canvases[canvas_index].demo) {
        case DEMO_WIREFRAME: draw_demo_wireframe(&app, canvas_index); break;
        case DEMO_GRADIENT: draw_demo_gradient(&app, canvas_index); break;
        case DEMO_CLOSED: draw_demo_closed(&app, canvas_index); break;
        case DEMO_ENDCAP: draw_demo_endcap(&app, canvas_index); break;
        case DEMO_NOISY: draw_demo_noisy(&app, canvas_index); break;
        case DEMO_STREAMLINES: draw_demo_streamlines(&app, canvas_index); break;
        case DEMO_CURVES: draw_demo_curves(&app, canvas_index); break;
    }
}

const char* get_vertex_shader(demo_type demo) { 
    switch (demo) {
        case DEMO_WIREFRAME:
            return parsh_get_blocks(app.shaders, "vsprefix wireframe.vs");
        case DEMO_GRADIENT:
            return parsh_get_blocks(app.shaders, "vsprefix gradient.vs");
        case DEMO_CLOSED:
            return parsh_get_blocks(app.shaders, "vsprefix closed.vs");
        case DEMO_ENDCAP:
            return parsh_get_blocks(app.shaders, "vsprefix endcap.vs");
        case DEMO_NOISY:
            return parsh_get_blocks(app.shaders, "vsprefix noisy.vs");
        case DEMO_STREAMLINES:
            return parsh_get_blocks(app.shaders, "vsprefix streamlines.vs");
        case DEMO_CURVES:
            return parsh_get_blocks(app.shaders, "vsprefix curves.vs");
    }
}

const char* get_fragment_shader(demo_type demo) {
    switch (demo) {
        case DEMO_WIREFRAME:
            return parsh_get_blocks(app.shaders, "fsprefix wireframe.fs");
        case DEMO_GRADIENT:
            return parsh_get_blocks(app.shaders, "fsprefix gradient.fs");
        case DEMO_CLOSED:
            return parsh_get_blocks(app.shaders, "fsprefix closed.fs");
        case DEMO_ENDCAP:
            return parsh_get_blocks(app.shaders, "fsprefix endcap.fs");
        case DEMO_NOISY:
            return parsh_get_blocks(app.shaders, "fsprefix noisy.fs");
        case DEMO_STREAMLINES:
            return parsh_get_blocks(app.shaders, "fsprefix streamlines.fs");
        case DEMO_CURVES:
            return parsh_get_blocks(app.shaders, "fsprefix curves.fs");
    }
}
