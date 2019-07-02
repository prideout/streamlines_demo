#define PAR_STREAMLINES_IMPLEMENTATION
#define SOKOL_GLES3
#define SOKOL_IMPL

#include <emscripten.h>
#include <GLES3/gl3.h>

#include <par/par_streamlines.h>

#include <sokol/sokol_gfx.h>
#include <sokol/sokol_time.h>

#include "demo.h"

app_state app;

EM_JS(int, init_gl, (int canvas_index), {
    const options = {
        alpha: false,
        depth: false,
        antialias: true,
        preserveDrawingBuffer: true // allows screenshots
    };

    const els = document.getElementsByTagName("canvas");
    const canvas = els[canvas_index];

    const ctx = canvas.getContext("webgl2", options);
    if (!ctx) {
        return 0;
    }

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
    if (init_gl(0) == 0) {
        EM_ASM_INT({ invoke_fallback() }, 0);
    } else {
        sg_setup(&(sg_desc){0});
        stm_setup();
        EM_ASM_INT({ start_all_demos() }, 0);
    }
}

void start(demo_type demo_index, int canvas_index, int variant) {
    assert(canvas_index < CANVAS_COUNT);
    canvas_state* canvas = &app.canvases[canvas_index];
    canvas->em_context = init_gl(canvas_index);
    canvas->gfx_context = sg_setup_context();
    canvas->demo_variant = variant;
    update_dims(canvas_index);
    init_common(demo_index, canvas_index);
    canvas->start_time = stm_now();
}

void draw(int canvas_index, bool completely_visible) {
    canvas_state* canvas = &app.canvases[canvas_index];

    // Unpause
    if (completely_visible && canvas->paused_time > 0) {
        uint64_t elapsed = stm_diff(canvas->paused_time, canvas->start_time);
        canvas->start_time = stm_now() - elapsed;
        canvas->paused_time = 0;
    }

    if (completely_visible || !canvas->has_drawn) {
        make_current(canvas->em_context);
        sg_activate_context(canvas->gfx_context);
        draw_common(canvas_index);
        canvas->has_drawn = true;
    }

    // Pause
    if (!completely_visible && canvas->paused_time == 0) {
        canvas->paused_time = stm_now();
    }
}
