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

EM_JS(void, init_gl, (), {
    const options = {};
    const canvas = document.getElementById('glcanvas');
    const ctx = canvas.getContext("webgl2", options);
    const handle = GL.registerContext(ctx, options);
    GL.makeContextCurrent(handle);

    const dpr = window.devicePixelRatio;
    canvas.width = canvas.clientWidth * dpr;
    canvas.height = canvas.clientHeight * dpr;
});

void update_framebuffer_dims() {
    app.framebuffer_width = EM_ASM_INT({
        const canvas = document.getElementById('glcanvas');
        return canvas.width;
    }, 0);
    app.framebuffer_height = EM_ASM_INT({
        const canvas = document.getElementById('glcanvas');
        return canvas.height;
    }, 0);
    app.framebuffer_scale = EM_ASM_DOUBLE({
        return 1.0 / window.devicePixelRatio;
    }, 0);
}

int main(int argc, char* argv[]) {

    init_gl();
    update_framebuffer_dims();
    sg_setup(&(sg_desc){});
    stm_setup();

    app.pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };

    switch (app.current_demo) {
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

void draw() {
    switch (app.current_demo) {
        case DEMO_SIMPLE: draw_demo_simple(&app); break;
        case DEMO_WIREFRAME: draw_demo_wireframe(&app); break;
        case DEMO_GRADIENT: draw_demo_gradient(&app); break;
        case DEMO_CLOSED: draw_demo_closed(&app); break;
        case DEMO_ENDCAP: draw_demo_endcap(&app); break;
        case DEMO_NOISY: draw_demo_noisy(&app); break;
        case DEMO_STREAMLINES: draw_demo_streamlines(&app); break;
    }
}
