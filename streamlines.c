#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"

#define PAR_STREAMLINES_IMPLEMENTATION
#include <par/par_streamlines.h>

#include "streamlines.h"

app_state app;

void init_app() {
    sg_setup(&(sg_desc){});
    stm_setup();

    app.pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };

    init_demo_simple(&app);

    app.start_time = stm_now();
}

void frame() {
    draw_demo_simple(&app);
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
    return (sapp_desc){
        .init_cb = init_app,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = handler,
        .width = 600,
        .height = 300,
        .sample_count = 16,
        .window_title = "streamlines",
    };
}
