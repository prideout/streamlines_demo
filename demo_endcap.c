#include "sokol_app.h"
#include "sokol_time.h"

#include "streamlines.h"

#include <assert.h>
#include <math.h>

#define DEMO_INDEX ((int) DEMO_ENDCAP)

static par_streamlines_position vertices[] = {
    {50, 150},
    {200, 100},
    {550, 200},

    {400, 200},
    {400, 100},
};

static uint16_t spine_lengths[] = { 3, 2 };

void init_demo_endcap(app_state* app) {

    demo_state* state = &app->demos[DEMO_INDEX];
    par_streamlines_config config = { .thickness = 15 };
    config.u_mode = PAR_U_MODE_DISTANCE;

    state->context = par_streamlines_create_context(config);

    state->spines = (par_streamlines_spine_list) {
        .num_vertices = sizeof(vertices) / sizeof(par_streamlines_position),
        .num_spines = sizeof(spine_lengths) / sizeof(uint16_t),
        .vertices = vertices,
        .spine_lengths = spine_lengths
    };
    par_streamlines_mesh* mesh;
    mesh = par_streamlines_draw_lines(state->context, state->spines);

    state->num_elements = mesh->num_triangles * 3;
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

    state->lengths_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = mesh->num_vertices * sizeof(float),
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
            "layout(location=2) in float length;\n"
            "out vec4 varying_annotation;\n"
            "out float varying_length;\n"
            "void main() {\n"
            "  vec2 p = 2.0 * position * resolution.xy - 1.0;"
            "  gl_Position = vec4(p, 0.0, 1.0);\n"
            "  varying_annotation = annotation;\n"
            "  varying_length = length;\n"
            "}\n",
        .fs.source =
            "#version 330\n"
            "in vec4 varying_annotation;\n"
            "in float varying_length;\n"
            "out vec4 frag_color;\n"
            "const float radius = 7.5;\n"
            "const float radius2 = 7.5 * 7.5;\n"
            "void main() {\n"
            "  float dist1 = abs(varying_annotation.x);\n"
            "  float dist2 = varying_length - abs(varying_annotation.x);\n"
            "  float dist = min(dist1, dist2);\n"
            "  float alpha = 1.0;\n"
            "  if (dist < radius) {\n"
            "      float x = dist - radius;\n"
            "      float y = varying_annotation.y * radius;\n"
            "      float d2 = x * x + y * y;\n"
            "      float t = fwidth(d2);\n"
            "      alpha = smoothstep(radius2 + t, radius2, d2);\n"
            "  }\n"
            "  frag_color = vec4(0, 0, 0, alpha);\n"
            "}\n"
    });

    state->bindings = (sg_bindings) {
        .vertex_buffers[0] = state->positions_buffer,
        .vertex_buffers[1] = state->annotations_buffer,
        .vertex_buffers[2] = state->lengths_buffer,
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

void draw_demo_endcap(app_state* app) {
    const double elapsed_seconds = stm_sec(stm_since(app->start_time));

    uniform_params resolution = {
        1.0f / sapp_width(),
        1.0f / sapp_height(),
        sapp_width(),
        sapp_height()
    };

    demo_state* state = &app->demos[DEMO_INDEX];

    vertices[1].y = 150 + 100 * sin(M_PI * elapsed_seconds);
    vertices[3].x = 400 + 50  * cos(M_PI * elapsed_seconds);
    vertices[3].y = 150 + 50  * sin(M_PI * elapsed_seconds);
    vertices[4].x = 400 - 50  * cos(M_PI * elapsed_seconds);
    vertices[4].y = 150 - 50  * sin(M_PI * elapsed_seconds);

    par_streamlines_mesh* mesh;
    mesh = par_streamlines_draw_lines(state->context, state->spines);

    sg_update_buffer(state->positions_buffer,
        mesh->vertex_positions,
        mesh->num_vertices * sizeof(par_streamlines_position));

    sg_update_buffer(state->annotations_buffer,
        mesh->vertex_annotations,
        mesh->num_vertices * sizeof(par_streamlines_annotation));

    sg_update_buffer(state->lengths_buffer,
        mesh->vertex_lengths,
        mesh->num_vertices * sizeof(float));

    sg_begin_default_pass(&app->pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&state->bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &resolution, sizeof(resolution));
    sg_draw(0, state->num_elements, 1);
    sg_end_pass();
    sg_commit();
}