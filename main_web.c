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

EM_JS(int, init_gl, (int canvas_index), {
    const options = {};

    const els = document.getElementsByTagName("canvas");
    const canvas = els[canvas_index];

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

void update_dims(int canvas_index) {
    app.width = EM_ASM_INT({
        return document.getElementsByTagName("canvas")[$0].width;
    }, canvas_index);
    app.height = EM_ASM_INT({
        return document.getElementsByTagName("canvas")[$0].height;
    }, canvas_index);
    app.pixel_ratio = EM_ASM_DOUBLE({
        // For now we are using half-size canvases.
        return 2.0 / window.devicePixelRatio;
    }, 0);
}

int main(int argc, char* argv[]) {
    init_gl(0);
    sg_setup(&(sg_desc){0});
    stm_setup();
    EM_ASM_INT({ start_all_demos() }, 0);
    app.start_time = stm_now();
}

void start(demo_type demo_index, int canvas_index, int variant) {
    app.canvases[canvas_index].em_context = init_gl(canvas_index);
    app.canvases[canvas_index].gfx_context = sg_setup_context();
    app.canvases[canvas_index].demo_variant = variant;
    update_dims(canvas_index);
    init_common(demo_index, canvas_index);
}

void draw(demo_type demo_index, int canvas_index) {
    make_current(app.canvases[canvas_index].em_context);
    sg_activate_context(app.canvases[canvas_index].gfx_context);
    draw_common(demo_index, canvas_index);
}
