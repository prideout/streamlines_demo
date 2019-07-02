#include <sokol/sokol_time.h>

#include "demo.h"

#include <assert.h>
#include <math.h>

#define DEMO_INDEX ((int) DEMO_STREAMLINES)

const float friction = 1.0f;
const float g = 1.0f;
const float L = 1.0f;
const float dt = 2.0;
const int initial_frames = 0;
const int frame_count = 100;

typedef struct {
    float inverse_width;
    float inverse_height;
    float width;
    float height;
    float time;
    float variant;
} uniforms;

static void advect(parsl_position* point, void* userdata) {
    app_state* app = userdata;
    float x = 8.0f * (point->x / (app->width * app->pixel_ratio) - 0.5f);
    float y = 4.0f * (point->y / (app->height * app->pixel_ratio) - 0.5f);
    const float theta = x;
    const float omega = y;
    const float omega_dot = -friction * omega - g / L * sin(theta);
    point->x += dt * omega;
    point->y += dt * omega_dot;
}

void init_demo_streamlines(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    parsl_u_mode u_mode;
    switch (state->demo_variant) {
        case 0: u_mode = PAR_U_MODE_NORMALIZED_DISTANCE; break;
        case 1: u_mode = PAR_U_MODE_SEGMENT_FRACTION; break;
    }

    const int margin = 50 * app->pixel_ratio;

    parsl_config config = {
        .thickness = 3,
        .streamlines_seed_spacing = 20,
        .streamlines_seed_viewport = {
            -margin, -margin,
            app->width * app->pixel_ratio + margin,
            app->height * app->pixel_ratio + margin
        },
        .flags = PARSL_FLAG_ANNOTATIONS | PARSL_FLAG_RANDOM_OFFSETS,
        .u_mode = u_mode,
    };

    state->context = parsl_create_context(config);

    parsl_mesh* mesh = parsl_mesh_from_streamlines(state->context, advect,
        initial_frames, frame_count, app);

    state->num_elements = mesh->num_triangles * 3;

    state->positions_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(parsl_position),
        .usage = SG_USAGE_IMMUTABLE,
        .content = mesh->positions
    });

    state->annotations_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(parsl_annotation),
        .usage = SG_USAGE_IMMUTABLE,
        .content = mesh->annotations
    });

    state->offsets_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(float),
        .usage = SG_USAGE_IMMUTABLE,
        .content = mesh->random_offsets
    });

    sg_buffer indices = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_triangles * 3 * sizeof(uint32_t),
        .usage = SG_USAGE_IMMUTABLE,
        .content = mesh->triangle_indices,
        .type = SG_BUFFERTYPE_INDEXBUFFER
    });

    parsl_destroy_context(state->context);

    sg_shader program = sg_make_shader(&(sg_shader_desc){
        .vs.uniform_blocks[0] = {
            .size = sizeof(uniforms),
            .uniforms = { 
                [0] = { .name = "resolution", .type = SG_UNIFORMTYPE_FLOAT4 },
                [1] = { .name = "time", .type = SG_UNIFORMTYPE_FLOAT },
                [2] = { .name = "variant", .type = SG_UNIFORMTYPE_FLOAT }
            }
        },
        .vs.source = get_vertex_shader(DEMO_INDEX),
        .fs.source = get_fragment_shader(DEMO_INDEX)
    });

    state->bindings = (sg_bindings) {
        .vertex_buffers[0] = state->positions_buffer,
        .vertex_buffers[1] = state->annotations_buffer,
        .vertex_buffers[2] = state->offsets_buffer,
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
                [2].buffer_index = 2,
                [2].offset = 0,
                [2].format = SG_VERTEXFORMAT_FLOAT,
            }
        }
    });
}

void draw_demo_streamlines(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    const double elapsed_seconds = stm_sec(stm_since(state->start_time));

    float scale = app->pixel_ratio;
    uniforms block = {
        1.0 / (scale * app->width),
        1.0 / (scale * app->height),
        scale * app->width,
        scale * app->height,
        (float) elapsed_seconds,
        (float) app->canvases[canvas_index].demo_variant
    };

    sg_begin_default_pass(&state->pass_action, app->width, app->height);
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&state->bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &block, sizeof(block));
    sg_draw(0, state->num_elements, 1);
    sg_end_pass();
    sg_commit();
}
