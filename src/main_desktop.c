#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"

#define PAR_STREAMLINES_IMPLEMENTATION
#include <par/par_streamlines.h>

#include "demo.h"

app_state app;

static const int demo_index = DEMO_WIREFRAME;
static const int variant_index = 0;

static const int canvas_index = 0; // must be zero in the desktop app

void init_app() {
    canvas_state* canvas = &app.canvases[canvas_index];
    sg_setup(&(sg_desc){});
    stm_setup();
    canvas->demo_variant = variant_index;
    init_common(demo_index, canvas_index);
    canvas->start_time = stm_now();
}

void frame() {
    draw_common(canvas_index);
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
