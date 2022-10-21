#include "complex2/fft.h"
#include "fft_gl.h"
#include "summation_gl.h"
#include "simulation.h"


static const float PI = 3.141592653589793;


void init_sim_params(struct SimParams *sim_params) {
    int width = START_WIDTH;
    int height = START_HEIGHT;
    // sim_params->enable_spin_interaction = 1;
    sim_params->steps_per_frame = (width >= 256)? 2: 5;
    sim_params->normalize_after_each_step = 0;
    // sim_params->steps_per_frame = 1;
    sim_params->dt = 1.0;
    sim_params->m = 1.0, sim_params->hbar = 1.0, sim_params->c = 137.036;
    sim_params->q = 1.0;
    sim_params->init_wavepacket.total_density = 741.0;
    sim_params->texel_width = width;
    sim_params->texel_height = height;
    sim_params->width = (float)width;
    sim_params->height = (float)height;
    sim_params->dx = sim_params->width/(float)width;
    sim_params->dy = sim_params->height/(float)height;
    sim_params->init_wavepacket.amplitude = 1.0;
    sim_params->init_wavepacket.sigma_x = 0.06;
    sim_params->init_wavepacket.sigma_y = 0.06;
    sim_params->init_wavepacket.u0 = 0.25;
    sim_params->init_wavepacket.v0 = 0.25;
    sim_params->init_wavepacket.nx = 0; //-10;
    sim_params->init_wavepacket.ny = 0; //10;
    sim_params->init_wavepacket.spin.c2[0] = 1.0;
    sim_params->init_wavepacket.spin.c2[1] = 0.0;
    sim_params->init_wavepacket.spin_direction.x = 0.0;
    sim_params->init_wavepacket.spin_direction.y = 1.0;
    sim_params->init_wavepacket.spin_direction.z = 0.0;
    sim_params->preset_potential.which = 0;
    sim_params->preset_potential.x_amplitude = 2.0;
    sim_params->preset_potential.y_amplitude = 2.0;
}

void init_programs(struct Programs *programs) {
    programs->zero = make_program("./shaders/zero.frag");
    programs->copy = make_program("./shaders/copy.frag");
    programs->copy2 = make_program("./shaders/copy2.frag");
    programs->resize_copy = make_program("./shaders/resize-copy.frag");
    programs->scale = make_program("./shaders/scale.frag");
    programs->complex_multiply
        = make_program("./shaders/complex-multiply.frag");
    programs->init_dist = make_program("./shaders/init-dist.frag");
    programs->add_to_density
        = make_program("./shaders/add-to-density.frag");
    programs->kinetic_step
        = make_program("./shaders/kinetic-step.frag");
    programs->potential_step
        = make_program("./shaders/potential-step.frag");
    programs->current = make_program("./shaders/current.frag");
    programs->pressure_iter = make_program("./shaders/pressure-iter.frag");
    programs->project_pressure
        = make_program("./shaders/project-pressure.frag");
    programs->del_dot_current
        = make_program("./shaders/del-dot-current.frag");
    programs->preset_potential
        = make_program("./shaders/preset-potential.frag");
    programs->preset_magnetic_field
        = make_program("./shaders/preset-magnetic-field.frag");
    programs->curl = make_program("./shaders/curl.frag");
    programs->advection = make_program("./shaders/advection.frag");
    programs->fft_iter = make_program("./shaders/fft-iter.frag");
    programs->rearrange = make_program("./shaders/rearrange.frag");
    programs->fftshift = make_program("./shaders/fftshift.frag");
    programs->dual_view = make_program("./shaders/dual-view.frag");
    programs->view = make_program("./shaders/view.frag");
}

void normalize_then_scale(struct SimParams *params,
                          const struct Programs *programs,
                          struct Frames *quads, float scale_val) {
    bind_quad(quads->current, programs->current);
    set_float_uniform("hbar", params->hbar);
    set_uniforms_dx_dy_width_height(params->dx, params->dy,
                                    params->width,
                                    params->height);
    set_sampler2D_uniform("wavefuncTex", quads->wavefunc[0]);
    draw_unbind();
    struct Vec4 tmpv = texture_reduction_sum(programs->scale,
                                             quads->current,
                                             quads->summation, START_WIDTH);
    bind_quad(quads->wavefunc[1], programs->scale);
    set_float_uniform("scale", scale_val/sqrt(tmpv.ind[3]));
    set_sampler2D_uniform("tex", quads->wavefunc[0]);
    draw_unbind();
    frame_id tmp = quads->wavefunc[0];
    quads->wavefunc[0] = quads->wavefunc[1];
    quads->wavefunc[1] = tmp;
    // get_texture_array(quads.current, 0, 0, 256, 256, GL_FLOAT,
    //                   (void *)tmp_arr);
    // struct Vec4 s2 = sum_vec4_array(256, 256, tmp_arr);
    // fprintf(stdout, "%f, %f\n", s.ind[3], s2.ind[3]);
    // fprintf(stdout, "%f\n", s.ind[3]);
}


void init_frames(struct Frames *quads, const struct SimParams *params) {
    int width = params->texel_width, height = params->texel_height;
    struct TextureParams texture_params = {
        .type=GL_FLOAT,
        .width=width, .height=height,
        .generate_mipmap=1,
        .wrap_s=GL_REPEAT, .wrap_t=GL_REPEAT,
        .min_filter=GL_LINEAR, .mag_filter=GL_LINEAR,
    };
    quads->main_view = new_quad(NULL);
    for (int i = 0; i < SUB_VIEW_ARR_SIZE; i++) {
        if (i >= 1) {
            texture_params.min_filter = GL_NEAREST;
            texture_params.mag_filter = GL_NEAREST;
        }
        quads->sub_views[i] = new_quad(&texture_params);
    }
    texture_params.width = width, texture_params.height = height;
    for (int i = 0; i < SIM_ARR_SIZE; i++) {
        quads->simulation[i] = new_quad(&texture_params);
    }
    for (int i = 0; i < FFT_ARR_SIZE; i++) {
        quads->fft[i] = new_quad(&texture_params);
    }
    texture_params.min_filter = GL_LINEAR;
    texture_params.mag_filter = GL_LINEAR;
    for (int i = 0; i < COLOUR_DENSITY_SIZE; i++) {
        quads->colour_densities[i] = new_quad(&texture_params);
    }
    for (int i = 0; i < SUM_ARR_SIZE; i++) {
        texture_params.width /= 2;
        texture_params.height /= 2;
        quads->summation[i] = new_quad(&texture_params);
    }
}

void set_uniforms_dx_dy_width_height(float dx, float dy,
                                     float width, float height) {
    set_float_uniform("dx", dx);
    set_float_uniform("dy", dy);
    set_float_uniform("width", width);
    set_float_uniform("height", height);
}

void init_wavepacket(GLuint init_wavepacket_program,
                     const struct SimParams *params, frame_id frame) {
    bind_quad(frame, init_wavepacket_program);
    set_float_uniform("amplitude", params->init_wavepacket.amplitude);
    set_float_uniform("sigma_x", params->init_wavepacket.sigma_x);
    set_float_uniform("sigma_y", params->init_wavepacket.sigma_y);
    set_float_uniform("u0", params->init_wavepacket.u0);
    set_float_uniform("v0", params->init_wavepacket.v0);
    set_float_uniform("nx", params->init_wavepacket.nx);
    set_float_uniform("ny", params->init_wavepacket.ny);
    set_vec4_uniform("spin",
                     params->init_wavepacket.spin.re1,
                     params->init_wavepacket.spin.im1,
                     params->init_wavepacket.spin.re2,
                     params->init_wavepacket.spin.im2);
    draw_unbind();
}

void add_to_colour_density(GLuint add_to_density_program,
                           const struct SimParams *params,
                           frame_id prev, frame_id next,
                           float r, float g, float b) {
    bind_quad(next, add_to_density_program);
    set_sampler2D_uniform("tex", prev);
    set_float_uniform("amplitude", 2.0*params->init_wavepacket.amplitude);
    set_float_uniform("sx", params->init_wavepacket.sigma_x);
    set_float_uniform("sy", params->init_wavepacket.sigma_y);
    set_float_uniform("uc", params->init_wavepacket.u0);
    set_float_uniform("vc", params->init_wavepacket.v0);
    set_float_uniform("r", r);
    set_float_uniform("g", g);
    set_float_uniform("b", b);
    draw_unbind();
}

void swap3(frame_id *f1, frame_id *f2, frame_id *f3) {
    frame_id tmp1 = *f1, tmp2 = *f2, tmp3 = *f3;
    *f1 = tmp2, *f2 = tmp3, *f3 = tmp1;
}

frame_id ft(const struct SimParams *params,
            const struct Programs *programs,
            const struct Frames *quads, frame_id initial) {
    reverse_bit_sort(programs->rearrange, quads->bit_sort_table,
                     initial, quads->fft_iter2[0]);
    frame_id res1 = fft_iter(programs->fft_iter,
                             quads->fft_iter2[0], quads->fft_iter2[1],
                             params->texel_height, 0);
    frame_id tmp1, tmp2;
    if (res1 == quads->fft_iter2[0])
        tmp1 = quads->fft_iter2[0], tmp2 = quads->fft_iter2[1];
    else
        tmp1 = quads->fft_iter2[1], tmp2 = quads->fft_iter2[0];
    return fft_iter(programs->fft_iter, tmp1, tmp2, params->texel_height, 1);
}

frame_id ift(const struct SimParams *params,
             const struct Programs *programs,
             const struct Frames *quads, frame_id initial) {
    // TODO: there is a bug if the initial frame is one of fft_iter frames
    reverse_bit_sort(programs->rearrange, quads->bit_sort_table,
                     initial, quads->fft_iter2[0]);
    frame_id res1 = ifft_iter(programs->fft_iter,
                              quads->fft_iter2[0], quads->fft_iter2[1],
                              params->texel_height, 0);
    frame_id tmp1, tmp2;
    if (res1 == quads->fft_iter2[0])
        tmp1 = quads->fft_iter2[0], tmp2 = quads->fft_iter2[1];
    else
        tmp1 = quads->fft_iter2[1], tmp2 = quads->fft_iter2[0];
    return ifft_iter(programs->fft_iter, tmp1, tmp2, params->texel_height, 1);
}

void project_pressure(const struct SimParams *params,
                      const struct Programs *programs,
                      const struct Frames *quads) {
    compute_del_dot_current(params, programs, quads, quads->wavefunc[1]);
    frame_id iter_quads[2] = {quads->pressure_iter2[0],
        quads->pressure_iter2[1]};
    for (int i = 0; i < 10; i++) {
        bind_quad(iter_quads[1], programs->pressure_iter);
        set_uniforms_dx_dy_width_height(params->dx, params->dy,
                                        params->width, params->height);
        set_sampler2D_uniform("bTex", quads->del_dot_current);
        set_sampler2D_uniform("lastIterTex", iter_quads[0]);
        draw_unbind();
        frame_id tmp = iter_quads[1];
        iter_quads[1] = iter_quads[0];
        iter_quads[0] = tmp;
    }
    // Project the pressure
    bind_quad(quads->wavefunc[0], programs->project_pressure);
    set_float_uniform("hbar", params->hbar);
    set_sampler2D_uniform("pressureTex", iter_quads[0]);
    set_sampler2D_uniform("wavefuncTex", quads->wavefunc[1]);
    draw_unbind();
}

void advection(const struct SimParams *params,
               const struct Programs *programs,
               const struct Frames *quads) {
    bind_quad(quads->colour_densities[1], programs->advection);
    set_float_uniform("dt", params->dt);
    set_float_uniform("width", params->width);
    set_float_uniform("height", params->height);
    set_sampler2D_uniform("velocityTex", quads->current);
    set_sampler2D_uniform("densityTex", quads->colour_densities[0]);
    draw_unbind();
}

frame_id timestep(const struct SimParams *params,
                  const struct Programs *programs,
                  const struct Frames *quads) {
    struct Complex2 dt;
    dt.c2[0] = params->dt; // *(1.0 - 0.1*I); // sqrt(2.0);
    dt.c2[1] = params->dt; // *(1.0 - 0.1*I); // sqrt(2.0);
    // Potential step
    bind_quad(quads->wavefunc[1], programs->potential_step);
    set_vec4_uniform("dt", dt.ind[0], dt.ind[1], dt.ind[2], dt.ind[3]);
    // set_float_uniform("m", params->m);
    set_float_uniform("hbar", params->hbar);
    set_sampler2D_uniform("wavefuncTex", quads->wavefunc[0]);
    set_sampler2D_uniform("potentialTex", quads->potential);
    draw_unbind();
    // Kinetic step
    frame_id res = ft(params, programs, quads, quads->wavefunc[1]);
    bind_quad(quads->wavefunc_p, programs->kinetic_step);
    set_vec4_uniform("dt", dt.ind[0], dt.ind[1], dt.ind[2], dt.ind[3]);
    set_float_uniform("m", params->m);
    set_float_uniform("hbar", params->hbar);
    set_float_uniform("q", params->q);
    set_float_uniform("c", params->c);
    // set_uniforms_dx_dy_width_height(params->dx, params->dy,
    //                                 params->width, params->height);
    set_float_uniform("pixelWidth", (float)params->texel_width);
    set_float_uniform("pixelHeight", (float)params->texel_height);
    float f = (1.0/250.0);
    float t = params->t;
    float s = 0.75*params->c*sin(2.0*PI*f*t);
    float c = 0.75*params->c*cos(2.0*PI*f*t);
    s = 0.0, c = 0.0;
    set_vec3_uniform("vecPotential", -s, c, 0.0);
    set_float_uniform("vecPotentialSquared", s*s + c*c);
    set_sampler2D_uniform("wavefuncTex", res);
    draw_unbind();
    frame_id res2 = ift(params, programs, quads, quads->wavefunc_p);
    // Potential step
    bind_quad(quads->wavefunc[1], programs->potential_step);
    set_vec4_uniform("dt", dt.ind[0], dt.ind[1], dt.ind[2], dt.ind[3]);
    // set_float_uniform("m", params->m);
    set_float_uniform("hbar", params->hbar);
    set_sampler2D_uniform("wavefuncTex", res2);
    set_sampler2D_uniform("potentialTex", quads->potential);
    set_int_uniform("setDensityUniform", 1);
    set_float_uniform("uniformDensityAmplitude", 1.0);
    draw_unbind();
    project_pressure(params, programs, quads);
    compute_current(params, programs, quads, quads->wavefunc[0]);
    advection(params, programs, quads);
    return quads->wavefunc[0];
}


void compute_current(const struct SimParams *params,
                     const struct Programs *programs,
                     const struct Frames *quads, frame_id wavefunc_tex) {
    bind_quad(quads->current, programs->current);
    set_float_uniform("hbar", params->hbar);
    set_uniforms_dx_dy_width_height(params->dx, params->dy,
                                    params->width, params->height);
    set_sampler2D_uniform("wavefuncTex", wavefunc_tex);
    draw_unbind();
}

void compute_del_dot_current(const struct SimParams *params,
                             const struct Programs *programs,
                             const struct Frames *quads,
                             frame_id wavefunc_tex) {
    bind_quad(quads->del_dot_current, programs->del_dot_current);
    set_float_uniform("hbar", params->hbar);
    set_uniforms_dx_dy_width_height(params->dx, params->dy,
                                    params->width, params->height);
    // set_sampler2D_uniform("normFactor");
    set_sampler2D_uniform("wavefuncTex", wavefunc_tex);
    set_int_uniform("isDensityConstant", 1);
    set_sampler2D_uniform("wavefuncTex", wavefunc_tex);
    draw_unbind();
}
