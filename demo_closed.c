#include "sokol_time.h"

#include "demo.h"

#include <assert.h>
#include <math.h>

#define SLICES 10
#define DEMO_INDEX ((int) DEMO_CLOSED)

static parsl_position vertices[SLICES + 6];

static uint16_t spine_lengths[] = { SLICES, 3, 3 };

void init_demo_closed(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    parsl_config config = { .thickness = 20 };

    state->context = parsl_create_context(config);

    state->spines = (parsl_spine_list) {
        .num_vertices = sizeof(vertices) / sizeof(parsl_position),
        .num_spines = sizeof(spine_lengths) / sizeof(uint16_t),
        .vertices = vertices,
        .spine_lengths = spine_lengths,
        .closed = true
    };
    parsl_mesh* mesh;
    mesh = parsl_mesh_from_lines(state->context, state->spines);

    state->positions_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(parsl_position),
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
        .vs.source = get_vertex_shader(DEMO_INDEX),
        .fs.source = get_fragment_shader(DEMO_INDEX),
    });

    state->bindings = (sg_bindings) {
        .vertex_buffers[0] = state->positions_buffer,
        .index_buffer = indices
    };

    state->pipeline = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = program,
        .blend = {
            .enabled = true,
            .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        },
        .index_type = SG_INDEXTYPE_UINT32,
        .layout = {
            .attrs = {
                [0].buffer_index = 0,
                [0].offset = 0,
                [0].format = SG_VERTEXFORMAT_FLOAT2
            }
        }
    });
}

void draw_demo_closed(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    const double elapsed_seconds = stm_sec(stm_since(state->start_time));

    float scale = app->pixel_ratio;
    uniform_params resolution = {
        1.0 / (scale * app->width),
        1.0 / (scale * app->height),
        scale * app->width,
        scale * app->height
    };

    const float dtheta = PI * 2 / SLICES;
    float theta = elapsed_seconds;
    for (int i = 0; i < SLICES; i++, theta += dtheta) {
        vertices[i].x = 300 + 100 * cos(theta);
        vertices[i].y = 150 + 100 * sin(theta);
    }

    vertices[SLICES + 0].x = 300+30;
    vertices[SLICES + 0].y = 150;
    vertices[SLICES + 1].x = 500+30;
    vertices[SLICES + 1].y = 100;
    vertices[SLICES + 2].x = 500+30;
    vertices[SLICES + 2].y = 190;

    vertices[SLICES + 3].x = 100-30;
    vertices[SLICES + 3].y = 190;
    vertices[SLICES + 4].x = 300-30;
    vertices[SLICES + 4].y = 150;
    vertices[SLICES + 5].x = 100-30;
    vertices[SLICES + 5].y = 100;

    parsl_mesh* mesh;
    mesh = parsl_mesh_from_lines(state->context, state->spines);

    sg_update_buffer(state->positions_buffer, mesh->positions,
        mesh->num_vertices * sizeof(parsl_position));

    sg_begin_default_pass(&state->pass_action, app->width, app->height);
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&state->bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &resolution, sizeof(resolution));
    sg_draw(0, mesh->num_triangles * 3, 1);
    sg_end_pass();
    sg_commit();
}
