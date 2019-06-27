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
    demo_type current_demo = EM_ASM_INT({ return window.current_demo }, 0);
    int current_variant = EM_ASM_INT({ return window.current_demo }, 0);
    app.variants[current_variant].em_context = init_gl();
    app.variants[current_variant].gfx_context = sg_setup_context();
    update_dims();
    init_common(current_demo, current_variant);
}

void draw() {
    demo_type current_demo = EM_ASM_INT({ return window.current_demo }, 0);
    int current_variant = EM_ASM_INT({ return window.current_demo }, 0);
    make_current(app.variants[current_variant].em_context);
    sg_activate_context(app.variants[current_variant].gfx_context);
    draw_common(current_demo, current_variant);

}
