#define PAR_STREAMLINES_IMPLEMENTATION
#define SOKOL_GLES3
#define SOKOL_IMPL

#include <emscripten.h>
#include <GLES3/gl3.h>

#include <par/par_streamlines.h>

#include "sokol_gfx.h"
#include "sokol_time.h"

#include "demo.h"

app_state app;

EM_JS(int, init_gl, (), {
    const options = {};
    const canvas = window.current_canvas;
    const ctx = canvas.getContext("webgl2", options);
    const handle = GL.registerContext(ctx, options);
    GL.makeContextCurrent(handle);

    const dpr = window.devicePixelRatio;
    canvas.width = canvas.clientWidth * dpr;
    canvas.height = canvas.clientHeight * dpr;

    return handle;
});

EM_JS(void, make_current, (int handle), {
    GL.makeContextCurrent(handle);
});

void update_dims() {
    app.width = EM_ASM_INT({
        return window.current_canvas.width;
    }, 0);
    app.height = EM_ASM_INT({
        return window.current_canvas.height;
    }, 0);
    app.pixel_ratio = EM_ASM_DOUBLE({
        // For now we are using half-size canvases.
        return 2.0 / window.devicePixelRatio;
    }, 0);
}

int main(int argc, char* argv[]) {
    init_gl();
    sg_setup(&(sg_desc){0});
    stm_setup();
    EM_ASM_INT({ start_all_demos() }, 0);
    app.start_time = stm_now();
}

void start() {
    int current_demo = EM_ASM_INT({ return window.current_demo }, 0);

    app.demos[current_demo].em_context = init_gl();
    app.demos[current_demo].gfx_context = sg_setup_context();
    app.demos[current_demo].pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };

    update_dims();

    switch (current_demo) {
        case DEMO_SIMPLE: init_demo_simple(&app); break;
        case DEMO_WIREFRAME: init_demo_wireframe(&app); break;
        case DEMO_GRADIENT: init_demo_gradient(&app); break;
        case DEMO_CLOSED: init_demo_closed(&app); break;
        case DEMO_ENDCAP: init_demo_endcap(&app); break;
        case DEMO_NOISY: init_demo_noisy(&app); break;
        case DEMO_STREAMLINES: init_demo_streamlines(&app); break;
    }
}

void draw() {
    int current_demo = EM_ASM_INT({ return window.current_demo }, 0);

    make_current(app.demos[current_demo].em_context);
    sg_activate_context(app.demos[current_demo].gfx_context);

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
