#include "sokol_time.h"

#include "demo.h"

#include <assert.h>
#include <math.h>

#define DEMO_INDEX ((int) DEMO_CURVES)

enum { VARIANT_CUBIC, VARIANT_QUADRATIC };

typedef struct {
    float inverse_width;
    float inverse_height;
    float width;
    float height;
    float apply_gradient;
} uniforms;

static parsl_position cubic_vertices[] = {
    { 10, 80 },   // P
    { 40, 10 },   // C1
    { 65, 10 },   // C2
    { 95, 80 },   // P
    { 150, 150 }, // C2
    { 180, 80 },  // P
};

static uint16_t cubic_spine_lengths[] = { 6 };

static parsl_position quadratic_vertices[] = {
    { 10, 80 },  // P
    { 95, 10 },  // C
    { 180, 80 }, // P
};

static uint16_t quadratic_spine_lengths[] = { 3 };

parsl_mesh* create_mesh(canvas_state* state) {
    switch (state->demo_variant) {
    case VARIANT_CUBIC:
        state->spines = (parsl_spine_list) {
            .num_vertices = sizeof(cubic_vertices) / sizeof(parsl_position),
            .num_spines = sizeof(cubic_spine_lengths) / sizeof(uint16_t),
            .vertices = cubic_vertices,
            .spine_lengths = cubic_spine_lengths
        };
        return parsl_mesh_from_curves_cubic(state->context, state->spines);
    case VARIANT_QUADRATIC:
        state->spines = (parsl_spine_list) {
            .num_vertices = sizeof(quadratic_vertices) / sizeof(parsl_position),
            .num_spines = sizeof(quadratic_spine_lengths) / sizeof(uint16_t),
            .vertices = quadratic_vertices,
            .spine_lengths = quadratic_spine_lengths
        };
        return parsl_mesh_from_curves_quadratic(state->context, state->spines);
    }
    return 0;
}

void init_demo_curves(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    parsl_config config = {
        .thickness = 8,
        .flags = PARSL_FLAG_ANNOTATIONS | PARSL_FLAG_CURVE_GUIDES
    };

    state->context = parsl_create_context(config);

    parsl_mesh* mesh = create_mesh(state);
    state->num_elements = mesh->num_triangles * 3;

    // The tessellation is dynamic so we need to overallocate.
    const int room_for_growth = 3;

    state->positions_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = room_for_growth * mesh->num_vertices * sizeof(parsl_position),
        .usage = SG_USAGE_DYNAMIC,
    });

    state->annotations_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = room_for_growth * mesh->num_vertices * sizeof(parsl_annotation),
        .usage = SG_USAGE_DYNAMIC,
    });

    state->index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = room_for_growth * mesh->num_triangles * 3 * sizeof(uint32_t),
        .usage = SG_USAGE_DYNAMIC,
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
        .index_buffer = state->index_buffer
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

void draw_demo_curves(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];
    const double elapsed_seconds = stm_sec(stm_since(state->start_time));

    float scale = app->pixel_ratio;
    uniforms block = {
        1.0 / (scale * app->width),
        1.0 / (scale * app->height),
        scale * app->width,
        scale * app->height,
        state->demo_variant ? 1.0f : 0.0f
    };

    // vertices[1].y = 150 + 100 * sin(PI * elapsed_seconds);
    // vertices[3].x = 400 + 50 * cos(PI * elapsed_seconds);
    // vertices[3].y = 150 + 50 * sin(PI * elapsed_seconds);
    // vertices[4].x = 400 - 50 * cos(PI * elapsed_seconds);
    // vertices[4].y = 150 - 50 * sin(PI * elapsed_seconds);

    cubic_vertices[2].x = 65 + 20 * sin(PI * elapsed_seconds);
    cubic_vertices[2].y = 10;

    parsl_mesh* mesh = create_mesh(state);
    state->num_elements = mesh->num_triangles * 3;

    sg_update_buffer(state->positions_buffer, mesh->positions,
        mesh->num_vertices * sizeof(parsl_position));

    sg_update_buffer(state->annotations_buffer, mesh->annotations,
        mesh->num_vertices * sizeof(parsl_annotation));

    sg_update_buffer(state->index_buffer, mesh->triangle_indices,
        mesh->num_triangles * 3 * sizeof(uint32_t));

    sg_begin_default_pass(&state->pass_action, app->width, app->height);
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&state->bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &block, sizeof(block));
    sg_draw(0, state->num_elements, 1);
    sg_end_pass();
    sg_commit();
}
