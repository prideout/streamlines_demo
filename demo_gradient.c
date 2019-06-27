#include "sokol_time.h"

#include "demo.h"

#include <assert.h>
#include <math.h>

#define DEMO_INDEX ((int) DEMO_GRADIENT)

typedef struct {
    float inverse_width;
    float inverse_height;
    float width;
    float height;
    float apply_gradient;
} uniforms;

static parsl_position vertices[] = {
    {50, 150},
    {200, 100},
    {550, 200},

    {400, 200},
    {400, 100},
};

static uint16_t spine_lengths[] = { 3, 2 };

void init_demo_gradient(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    parsl_config config = { .thickness = 15, .flags = PARSL_FLAG_ANNOTATIONS };

    state->context = parsl_create_context(config);

    state->spines = (parsl_spine_list) {
        .num_vertices = sizeof(vertices) / sizeof(parsl_position),
        .num_spines = sizeof(spine_lengths) / sizeof(uint16_t),
        .vertices = vertices,
        .spine_lengths = spine_lengths
    };
    parsl_mesh* mesh;
    mesh = parsl_mesh_from_lines(state->context, state->spines);

    state->num_elements = mesh->num_triangles * 3;
    assert(sizeof(parsl_position) == 2 * sizeof(float));
    assert(sizeof(parsl_annotation) == 4 * sizeof(float));

    state->positions_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(parsl_position),
        .usage = SG_USAGE_DYNAMIC,
    });

    state->annotations_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(parsl_annotation),
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
            .size = sizeof(uniforms),
            .uniforms = { 
                [0] = { .name = "resolution", .type = SG_UNIFORMTYPE_FLOAT4 },
                [1] = { .name = "apply_gradient", .type = SG_UNIFORMTYPE_FLOAT }
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

void draw_demo_gradient(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    const double elapsed_seconds = stm_sec(stm_since(app->start_time));

    float scale = app->pixel_ratio;
    uniforms block = {
        1.0 / (scale * app->width),
        1.0 / (scale * app->height),
        scale * app->width,
        scale * app->height,
        state->demo_variant ? 1.0f : 0.0f
    };

    vertices[1].y = 150 + 100 * sin(PI * elapsed_seconds);
    vertices[3].x = 400 + 50 * cos(PI * elapsed_seconds);
    vertices[3].y = 150 + 50 * sin(PI * elapsed_seconds);
    vertices[4].x = 400 - 50 * cos(PI * elapsed_seconds);
    vertices[4].y = 150 - 50 * sin(PI * elapsed_seconds);

    parsl_mesh* mesh = parsl_mesh_from_lines(state->context, state->spines);

    sg_update_buffer(state->positions_buffer, mesh->positions,
        mesh->num_vertices * sizeof(parsl_position));

    sg_update_buffer(state->annotations_buffer, mesh->annotations,
        mesh->num_vertices * sizeof(parsl_annotation));

    sg_begin_default_pass(&state->pass_action, app->width, app->height);
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&state->bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &block, sizeof(block));
    sg_draw(0, state->num_elements, 1);
    sg_end_pass();
    sg_commit();
}
