#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulation.h"


static void swap_frames(frame_id *f1, frame_id *f2) {
    frame_id tmp = *f1;
    *f1 = *f2;
    *f2 = tmp;
}

void init_programs(struct Programs *programs) {
    programs->add_forces = make_quad_program("./shaders/add-forces.frag");
    programs->advection = make_quad_program("./shaders/advection.frag");
    programs->copy = make_quad_program("./shaders/copy.frag");
    programs->copy2 = make_quad_program( "./shaders/copy2.frag");
    programs->diffusion_iter
        = make_quad_program("./shaders/diffusion-iter.frag");
    programs->divergence = make_quad_program( "./shaders/divergence.frag");
    programs->init_dist = make_quad_program("./shaders/init-dist.frag");
    programs->pressure_iter
        = make_quad_program("./shaders/pressure-iter.frag");
    programs->subtract_gradp
        = make_quad_program("./shaders/subtract-gradp.frag");
    programs->zero = make_quad_program("./shaders/zero.frag");
    programs->view = make_quad_program("./shaders/view.frag");
    programs->barrier_velocity
        = make_quad_program("./shaders/barrier-velocity.frag");
    programs->enforce_free_slip
        = make_quad_program("./shaders/enforce-free-slip.frag");
}

void init_frames(struct Frames *quads, const struct SimParams *sim_params,
                 int texel_width, int texel_height) {
    int wrap_mode = 0;
    if (sim_params->boundary_mode == 0) wrap_mode = GL_REPEAT;
    else if (sim_params->boundary_mode == 1) wrap_mode = GL_CLAMP_TO_EDGE;
    struct TextureParams texture_params = {
        .type=GL_HALF_FLOAT,
        .width=texel_width, .height=texel_height,
        .generate_mipmap=1,
        .wrap_s=wrap_mode, .wrap_t=wrap_mode,
        .min_filter=GL_LINEAR, .mag_filter=GL_LINEAR,
    };
    quads->all[0] = new_quad(NULL);
    for (size_t i = 1; i < NUMBER_OF_FRAMES; i++) {
        quads->all[i] = new_quad(&texture_params);
    }
}

void init_sim_params(struct SimParams *sim_params,
                     int texel_width, int texel_height) {
    sim_params->boundary_mode = 1;
    sim_params->dt = 0.1;
    sim_params->height = 1.0;
    sim_params->width
        = sim_params->height*(float)texel_width/(float)texel_height;
    sim_params->dx = sim_params->width/texel_width;
    sim_params->dy = sim_params->height/texel_height;
    sim_params->nu = 0.00005;
    sim_params->diff_type = 1.0;
    sim_params->diff_type = 0;
    sim_params->diffusion_iterations = 0;
    sim_params->pressure_iterations = 15;
}

void init_dist_params(struct DistParams *dist_params,
                      int texel_width, int texel_height) {
    dist_params->width = texel_width;
    dist_params->height = texel_height;
    dist_params->sy = 0.015;
    dist_params->sx = dist_params->sy*(float)texel_height/(float)texel_width;
    dist_params->r = 0.1;
    dist_params->g = 0.0;
    dist_params->b = 0.0;
    dist_params->amplitude = 16.0;
    dist_params->uc = 0.5;
    dist_params->vc = 0.5;
}

void init_dist(GLuint init_dist_program,
               const struct DistParams *params,
               frame_id quad) {
    bind_quad(quad, init_dist_program);
    set_float_uniform("amplitude", params->amplitude);
    set_float_uniform("width", params->width);
    set_float_uniform("height", params->height);
    set_float_uniform("uc", params->uc);
    set_float_uniform("vc", params->vc);
    set_float_uniform("sx", params->sx);
    set_float_uniform("sy", params->sy);
    set_float_uniform("r", params->r);
    set_float_uniform("g", params->g);
    set_float_uniform("b", params->b);
    draw_unbind_quad();
}

void dist_params_set_rgb(struct DistParams *dist_params,
                         float r, float g, float b) {
    dist_params->r = r;
    dist_params->b = b;
    dist_params->g = g;
}

void set_uniforms_dx_dy_width_height(float dx, float dy,
                                     float width, float height) {
    set_float_uniform("dx", dx);
    set_float_uniform("dy", dy);
    set_float_uniform("width", width);
    set_float_uniform("height", height);
}

static void advection(const struct SimParams *sim_params,
                      const struct Programs *programs,
                      const struct Frames *quads) {
    // Advection for densities
    bind_quad(quads->densities2[1], programs->advection);
    set_float_uniform("dt", sim_params->dt);
    set_int_uniform("advectorTex", quads->fluid_velocity);
    set_int_uniform("advecteeTex", quads->densities2[0]);
    draw_unbind_quad();
    // Advection for the velocity field itself.
    bind_quad(quads->advection_result, programs->advection);
    set_float_uniform("dt", sim_params->dt);
    set_int_uniform("advectorTex", quads->fluid_velocity);
    set_int_uniform("advecteeTex", quads->fluid_velocity);
    draw_unbind_quad();

    // Compute the fictitious field velocity at barrier texels to satisfy
    // boundary conditions between barriers and fluid.
    /*bind_quad(quads->advection_result, programs->barrier_velocity);
    set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                    sim_params->width, sim_params->height);
    set_sampler2D_uniform("tex", quads->tmps[0]);
    draw_unbind_quad();*/
}

static void diffusion(const struct SimParams *sim_params,
                      const struct Programs *programs,
                      const struct Frames *quads) {
    frame_id iter_quads[2] = {0.0,};
    //Diffusion
    if (sim_params->diffusion_iterations > 0) {
        iter_quads[0] = quads->diffusion_iter2[0];
        iter_quads[1] = quads->diffusion_iter2[1];
        for (size_t i = 0; i < sim_params->diffusion_iterations; i++) {
            bind_quad(iter_quads[1], programs->diffusion_iter);
            set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                            sim_params->width,
                                            sim_params->height);
            set_float_uniform("nu", sim_params->nu);
            set_float_uniform("dt", sim_params->dt);
            set_sampler2D_uniform("bTex", quads->advection_result);
            set_sampler2D_uniform("lastIterTex", iter_quads[0]);
            draw_unbind_quad();
            swap_frames(&iter_quads[0], &iter_quads[1]);
        }
        bind_quad(quads->diffusion_result, programs->copy);
        set_sampler2D_uniform("tex", iter_quads[0]);
        draw_unbind_quad();
    }
}

static void add_forces(const struct SimParams *sim_params,
                       const struct Programs *programs,
                       const struct Frames *quads) {
    frame_id pre_add_forces = (sim_params->diffusion_iterations > 0)?
        quads->diffusion_result: quads->advection_result;
    bind_quad(quads->add_forces_result, programs->add_forces);
    set_float_uniform("dt", sim_params->dt);
    set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                    sim_params->width, sim_params->height);
    set_int_uniform("boundaryMode", sim_params->boundary_mode);
    set_sampler2D_uniform("forceTex", quads->force);
    set_sampler2D_uniform("uTex", pre_add_forces);
    // print_user_defined_uniforms();
    draw_unbind_quad();
}

static void divergence_velocity(const struct SimParams *sim_params,
                                const struct Programs *programs,
                                const struct Frames *quads) {
    bind_quad(quads->div_velocity, programs->divergence);
    set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                    sim_params->width, sim_params->height);
    set_int_uniform("diffType", sim_params->diff_type);
    set_sampler2D_uniform("vectTex", quads->add_forces_result);
    draw_unbind_quad();
}

static void pressure(const struct SimParams *sim_params,
                     const struct Programs *programs,
                     const struct Frames *quads) {
    // pressure projection
    frame_id iter_quads[2] = {0,};
    iter_quads[0] = quads->pressure_iter2[0];
    iter_quads[1] = quads->pressure_iter2[1];
    for (size_t i = 0; i < sim_params->pressure_iterations; i++) {
        bind_quad(iter_quads[1], programs->pressure_iter);
        set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                        sim_params->width, sim_params->height);
        set_sampler2D_uniform("bTex", quads->div_velocity);
        set_sampler2D_uniform("lastIterTex", iter_quads[0]);
        draw_unbind_quad();
        swap_frames(&iter_quads[0], &iter_quads[1]);
    }
    // Subtract the gradient of the pressure
    bind_quad(quads->tmps[0], programs->subtract_gradp);
    set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                    sim_params->width, sim_params->height);
    set_sampler2D_uniform("pressureTex", iter_quads[0]);
    set_sampler2D_uniform("uTex", quads->add_forces_result);
    draw_unbind_quad();
    // Enforce the free slip condition
    bind_quad(quads->pressure_result, programs->enforce_free_slip);
    set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                    sim_params->width, sim_params->height);
    set_sampler2D_uniform("uTex", quads->tmps[0]);
    draw_unbind_quad();
}

void time_step(const struct SimParams *sim_params,
               const struct DistParams *dist_params,
               const struct Programs *programs,
               const struct Frames *quads) {
    advection(sim_params, programs, quads);

    diffusion(sim_params, programs, quads);
    add_forces(sim_params, programs, quads);
    divergence_velocity(sim_params, programs, quads);
    pressure(sim_params, programs, quads);

    bind_quad(quads->fluid_velocity, programs->copy);
    set_sampler2D_uniform("tex", quads->pressure_result);
    draw_unbind_quad();
}
