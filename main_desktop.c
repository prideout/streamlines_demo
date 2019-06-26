#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"

#define PAR_STREAMLINES_IMPLEMENTATION
#include <par/par_streamlines.h>

#include "demo.h"

app_state app;
int current_demo;

void init_app() {
    sg_setup(&(sg_desc){});
    stm_setup();

    app.demos[current_demo].pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };

    switch (current_demo) {
        case DEMO_SIMPLE: init_demo_simple(&app); break;
        case DEMO_WIREFRAME: init_demo_wireframe(&app); break;
        case DEMO_GRADIENT: init_demo_gradient(&app); break;
        case DEMO_CLOSED: init_demo_closed(&app); break;
        case DEMO_ENDCAP: init_demo_endcap(&app); break;
        case DEMO_NOISY: init_demo_noisy(&app); break;
        case DEMO_STREAMLINES: init_demo_streamlines(&app); break;
    }

    app.start_time = stm_now();
}

void frame() {
    switch (current_demo) {
        case DEMO_SIMPLE: draw_demo_simple(&app); break;
        case DEMO_WIREFRAME: draw_demo_wireframe(&app); break;
        case DEMO_GRADIENT: draw_demo_gradient(&app); break;
        case DEMO_CLOSED: draw_demo_closed(&app); break;
        case DEMO_ENDCAP: draw_demo_endcap(&app); break;
        case DEMO_NOISY: draw_demo_noisy(&app); break;
        case DEMO_STREAMLINES: draw_demo_streamlines(&app); break;
    }
}

void cleanup() {
    sg_shutdown();
}

void handler(const sapp_event* event) {
    if (event->type != SAPP_EVENTTYPE_KEY_UP) {
        return;
    }
    switch (event->key_code) {
        case SAPP_KEYCODE_ESCAPE:
        case SAPP_KEYCODE_Q:
            sapp_request_quit();
        default:
            break;
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    current_demo = DEMO_STREAMLINES;

    if (argc > 1) {
        int index = atoi(argv[1]);
        if (index >= 0 && index < DEMO_COUNT) {
            current_demo = index;
        }
    }

    app.width = 600;
    app.height = 300;
    app.pixel_ratio = 1.0f;

    return (sapp_desc){
        .init_cb = init_app,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = handler,
        .width = app.width,
        .height = app.height,
        .sample_count = 16,
        .window_title = "streamlines",
    };
}
