#define PAR_STREAMLINES_IMPLEMENTATION
#define SOKOL_GLES3
#define SOKOL_IMPL

#include <GLES3/gl3.h>

#include <par/par_streamlines.h>

#include "sokol_gfx.h"
#include "sokol_time.h"

#include "demo.h"

app_state app;

int main(int argc, char* argv[]) {
    switch (app.current_demo) {
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
