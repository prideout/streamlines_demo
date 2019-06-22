#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"

#define PAR_STREAMLINES_IMPLEMENTATION
#include <par/par_streamlines.h>

#include <sys/time.h>

par_streamlines_position vertices[] = {
    {50, 150},
    {200, 100},
    {550, 200},
};

uint16_t spine_lengths[] = { 3 };

typedef struct {
    float inverse_width;
    float inverse_height;
    float width;
    float height;
} uniform_params;

struct {
    sg_pass_action pass_action;
    par_streamlines_context* context;
    sg_pipeline pipeline;
    sg_bindings bindings;
    sg_buffer positions_buffer;
    sg_buffer annotations_buffer;
    int num_elements;
    uint64_t start_time;
    par_streamlines_spine_list spines;
} app;

void init() {
    sg_setup(&(sg_desc){});
    stm_setup();

    app.pass_action = (sg_pass_action) {
        .colors[0] = {
            .action = SG_ACTION_CLEAR,
            .val = {0.8f, 0.8f, 0.8f, 1.0f}
        }
    };

    par_streamlines_config config = { .thickness = 10 };
    app.context = par_streamlines_create_context(config);

    app.spines = (par_streamlines_spine_list) {
        .num_vertices = 3,
        .num_spines = 1,
        .vertices = vertices,
        .spine_lengths = spine_lengths
    };
    par_streamlines_mesh* mesh;
    mesh = par_streamlines_draw_lines(app.context, app.spines);

    app.num_elements = mesh->num_triangles * 3;
    assert(sizeof(par_streamlines_position) == 2 * sizeof(float));
    assert(sizeof(par_streamlines_annotation) == 4 * sizeof(float));

    app.positions_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(par_streamlines_position),
        .usage = SG_USAGE_DYNAMIC,
    });

    app.annotations_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(par_streamlines_annotation),
        .usage = SG_USAGE_DYNAMIC,
    });

    sg_buffer indices = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_triangles * 3 * sizeof(uint32_t),
        .usage = SG_USAGE_IMMUTABLE,
        .content = mesh->triangle_indices,
        .type = SG_BUFFERTYPE_INDEXBUFFER
    });

    sg_shader program = sg_make_shader(&(sg_shader_desc){
        .vs.uniform_blocks[0] = {
            .size = sizeof(uniform_params),
            .uniforms = { 
                [0] = { .name = "resolution", .type = SG_UNIFORMTYPE_FLOAT4 }
            }
        },
        .vs.source = 
            "#version 330\n"
            "uniform vec4 resolution\n;"
            "layout(location=0) in vec2 position;\n"
            "layout(location=1) in vec4 annotation;\n"
            "out vec4 vannotation;\n"
            "void main() {\n"
            "  vec2 p = 2.0 * position * resolution.xy - 1.0;"
            "  gl_Position = vec4(p, 0.0, 1.0);\n"
            "  vannotation = annotation;\n"
            "}\n",
        .fs.source =
            "#version 330\n"
            "in vec4 vannotation;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "  frag_color = vec4(0, 0, 0, 1);\n"
            "}\n"
    });

    app.bindings = (sg_bindings) {
        .vertex_buffers[0] = app.positions_buffer,
        .vertex_buffers[1] = app.annotations_buffer,
        .index_buffer = indices
    };

    app.pipeline = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = program,
        .index_type = SG_INDEXTYPE_UINT32,
        .layout = {
            .attrs = {
                [0].buffer_index = 0,
                [0].offset = 0,
                [0].format = SG_VERTEXFORMAT_FLOAT2,
                [1].buffer_index = 1,
                [1].offset = 0,
                [1].format = SG_VERTEXFORMAT_FLOAT4,
            }
        }
    });

    app.start_time = stm_now();
}

void frame() {
    const double elapsed_seconds = stm_sec(stm_since(app.start_time));

    uniform_params resolution = {
        1.0f / sapp_width(),
        1.0f / sapp_height(),
        sapp_width(),
        sapp_height()
    };

    vertices[1].y = 150 + 100 * sin(M_PI * elapsed_seconds);

    par_streamlines_mesh* mesh;
    mesh = par_streamlines_draw_lines(app.context, app.spines);

    sg_update_buffer(app.positions_buffer,
        mesh->vertex_positions,
        mesh->num_vertices * sizeof(par_streamlines_position));

    sg_update_buffer(app.annotations_buffer,
        mesh->vertex_annotations,
        mesh->num_vertices * sizeof(par_streamlines_annotation));

    sg_begin_default_pass(&app.pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(app.pipeline);
    sg_apply_bindings(&app.bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &resolution, sizeof(resolution));
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
        .sample_count = 16,
        .window_title = "streamlines",
    };
}
