#include "sokol_time.h"

#include "demo.h"

#include <assert.h>
#include <math.h>

#define SLICES 100
#define DEMO_INDEX ((int) DEMO_NOISY)

static par_streamlines_position vertices[SLICES * 2];

static uint16_t spine_lengths[] = { SLICES, SLICES };

void init_demo_noisy(app_state* app) {

    demo_state* state = &app->demos[DEMO_INDEX];
    par_streamlines_config config = { .thickness = 3 };

    state->context = par_streamlines_create_context(config);

    state->spines = (par_streamlines_spine_list) {
        .num_vertices = sizeof(vertices) / sizeof(par_streamlines_position),
        .num_spines = sizeof(spine_lengths) / sizeof(uint16_t),
        .vertices = vertices,
        .spine_lengths = spine_lengths,
        .closed = true
    };
    par_streamlines_mesh* mesh;
    mesh = par_streamlines_draw_lines(state->context, state->spines);

    assert(sizeof(par_streamlines_position) == 2 * sizeof(float));
    assert(sizeof(par_streamlines_annotation) == 4 * sizeof(float));

    state->positions_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(par_streamlines_position),
        .usage = SG_USAGE_DYNAMIC,
    });

    state->annotations_buffer = sg_make_buffer(&(sg_buffer_desc){
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
        .vs.source = get_vertex_shader(DEMO_INDEX),
        .fs.source = get_fragment_shader(DEMO_INDEX)
    });

    state->bindings = (sg_bindings) {
        .vertex_buffers[0] = state->positions_buffer,
        .vertex_buffers[1] = state->annotations_buffer,
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
                [0].format = SG_VERTEXFORMAT_FLOAT2,
                [1].buffer_index = 1,
                [1].offset = 0,
                [1].format = SG_VERTEXFORMAT_FLOAT4,
            }
        }
    });
}

void draw_demo_noisy(app_state* app) {
    const double elapsed_seconds = stm_sec(stm_since(app->start_time));

    float scale = app->framebuffer_scale;
    uniform_params resolution = {
        1.0 / (scale * app->framebuffer_width),
        1.0 / (scale * app->framebuffer_height),
        scale * app->framebuffer_width,
        scale * app->framebuffer_height
    };

    demo_state* state = &app->demos[DEMO_INDEX];

    const float dtheta = PI * 2 / SLICES;
    float theta = elapsed_seconds;
    for (int i = 0; i < SLICES; i++, theta += dtheta) {
        vertices[i].x = 300 + 100 * cos(theta);
        vertices[i].y = 150 + 100 * sin(theta);
    }

    float x = 100;
    const float dx = 400.0f / SLICES;
    for (int i = SLICES; i < SLICES * 2; i++, x += dx) {
        vertices[i].x = x;
        vertices[i].y = 150;
    }

    par_streamlines_mesh* mesh;
    mesh = par_streamlines_draw_lines(state->context, state->spines);

    sg_update_buffer(state->positions_buffer,
        mesh->vertex_positions,
        mesh->num_vertices * sizeof(par_streamlines_position));

    sg_update_buffer(state->annotations_buffer,
        mesh->vertex_annotations,
        mesh->num_vertices * sizeof(par_streamlines_annotation));

    sg_begin_default_pass(&app->pass_action, app->framebuffer_width, app->framebuffer_height);
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&state->bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &resolution, sizeof(resolution));
    sg_draw(0, mesh->num_triangles * 3, 1);
    sg_end_pass();
    sg_commit();
}
