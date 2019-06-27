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

EM_JS(int, init_gl, (int variant_index), {
    const options = {};

    const els = document.getElementsByTagName("canvas");
    const canvas = els[variant_index];

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

void update_dims(int variant_index) {
    app.width = EM_ASM_INT({
        return document.getElementsByTagName("canvas")[$0].width;
    }, variant_index);
    app.height = EM_ASM_INT({
        return document.getElementsByTagName("canvas")[$0].height;
    }, variant_index);
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

void start(demo_type current_demo, int current_variant) {
    app.variants[current_variant].em_context = init_gl(current_variant);
    app.variants[current_variant].gfx_context = sg_setup_context();
    update_dims(current_variant);
    init_common(current_demo, current_variant);
}

void draw(demo_type current_demo, int current_variant) {
    make_current(app.variants[current_variant].em_context);
    sg_activate_context(app.variants[current_variant].gfx_context);
    draw_common(current_demo, current_variant);
}
