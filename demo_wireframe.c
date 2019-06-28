#include "sokol_time.h"

#include "demo.h"

#include <assert.h>
#include <math.h>

#define DEMO_INDEX ((int) DEMO_WIREFRAME)

static parsl_position vertices[] = {
    {50, 150},
    {200, 100},
    {550, 200},

    {400, 200},
    {400, 100},
};

static uint16_t spine_lengths[] = { 3, 2 };

void init_demo_wireframe(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];

    parsl_config config = { .thickness = 15, .flags = PARSL_FLAG_WIREFRAME };

    state->context = parsl_create_context(config);

    state->spines = (parsl_spine_list) {
        .num_vertices = sizeof(vertices) / sizeof(parsl_position),
        .num_spines = sizeof(spine_lengths) / sizeof(uint16_t),
        .vertices = vertices,
        .spine_lengths = spine_lengths
    };
    parsl_mesh* mesh;
    mesh = parsl_mesh_from_lines(state->context, state->spines);

    state->num_elements = mesh->num_triangles * 4;

    state->positions_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(parsl_position),
        .usage = SG_USAGE_DYNAMIC,
    });

    sg_buffer indices = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_triangles * 4 * sizeof(uint32_t),
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
        .index_type = SG_INDEXTYPE_UINT32,
        .primitive_type = SG_PRIMITIVETYPE_LINE_STRIP,
        .layout = {
            .attrs = {
                [0].buffer_index = 0,
                [0].offset = 0,
                [0].format = SG_VERTEXFORMAT_FLOAT2
            }
        }
    });
}

void draw_demo_wireframe(app_state* app, int canvas_index) {
    canvas_state* state = &app->canvases[canvas_index];

    const double elapsed_seconds = stm_sec(stm_since(state->start_time));

    float scale = app->pixel_ratio;
    uniform_params resolution = {
        1.0 / (scale * app->width),
        1.0 / (scale * app->height),
        scale * app->width,
        scale * app->height
    };

    vertices[1].y = 150 + 100 * sin(PI * elapsed_seconds);
    vertices[3].x = 400 + 50 * cos(PI * elapsed_seconds);
    vertices[3].y = 150 + 50 * sin(PI * elapsed_seconds);
    vertices[4].x = 400 - 50 * cos(PI * elapsed_seconds);
    vertices[4].y = 150 - 50 * sin(PI * elapsed_seconds);

    parsl_mesh* mesh;
    mesh = parsl_mesh_from_lines(state->context, state->spines);

    sg_update_buffer(state->positions_buffer, mesh->positions,
        mesh->num_vertices * sizeof(parsl_position));

    sg_begin_default_pass(&state->pass_action, app->width, app->height);
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&state->bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &resolution, sizeof(resolution));
    for (int i = 0; i < state->num_elements; i += 4) {
        sg_draw(i, 4, 1);
    }
    sg_end_pass();
    sg_commit();
}
