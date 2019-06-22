#include "sokol_app.h"
#include "sokol_gfx.h"

#define PAR_STREAMLINES_IMPLEMENTATION
#include <par/par_streamlines.h>

struct {
    sg_pass_action pass_action;
    par_streamlines_context* context;
    sg_pipeline pipeline;
    sg_bindings bindings;
    int num_elements;
} app;

void init() {
    sg_setup(&(sg_desc){});

    app.pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };

    par_streamlines_config config = { .thickness = 3 };
    app.context = par_streamlines_create_context(config);

    par_streamlines_position vertices[] = {
        {50, 200},
        {300, 400},
        {550, 200},
    };
    uint16_t spine_lengths[] = { 3 };
    par_streamlines_spine_list spines = {
        .num_vertices = 3,
        .num_spines = 1,
        .vertices = vertices,
        .spine_lengths = spine_lengths
    };
    par_streamlines_mesh* mesh;
    mesh = par_streamlines_draw_lines(app.context, spines);
    app.num_elements = mesh->num_triangles * 3;

    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(par_streamlines_position),
        .content = mesh->vertex_positions
    });
    
    sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_triangles * 3 * sizeof(uint32_t),
        .content = mesh->triangle_indices,
        .type = SG_BUFFERTYPE_INDEXBUFFER
    });

    app.bindings = (sg_bindings) {
        .vertex_buffers[0] = vbuf,
        .index_buffer = ibuf
    };

    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        .attrs = {
            [0].name = "position",
            [1].name = "annotation"
        },
        .vs.source = 
            "#version 330\n"
            "in vec2 position;\n"
            "in vec4 annotation;\n"
            "out vec4 vannotation;\n"
            "void main() {\n"
            "  gl_Position = vec4(position, 0, 1);\n"
            "  vannotation = annotation;\n"
            "}\n",
        .fs.source =
            "#version 330\n"
            "in vec4 vannotation;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "  frag_color = vec4(1);\n"
            "}\n"
    });

    app.pipeline = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .index_type = SG_INDEXTYPE_UINT32,
        .layout = {
            .attrs = {
                [0].format=SG_VERTEXFORMAT_FLOAT2,
                [1].format=SG_VERTEXFORMAT_FLOAT4
            }
        }
    });
}

void frame() {
    sg_begin_default_pass(&app.pass_action, sapp_width(), sapp_height());

    sg_apply_pipeline(app.pipeline);
    sg_apply_bindings(&app.bindings);
    sg_draw(0, app.num_elements, 1);

    sg_end_pass();
    sg_commit();
}

void cleanup() {
    par_streamlines_destroy_context(app.context);
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
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = handler,
        .width = 600,
        .height = 300,
        .window_title = "streamlines",
    };
}
