#include "complex2/complex2.h"
#include "gl_wrappers/gl_wrappers.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SIMULATION_
#define _SIMULATION_

#define SQUARE_SIDE_LENGTH 256
#define MAX_WIDTH SQUARE_SIDE_LENGTH
#define MAX_HEIGHT SQUARE_SIDE_LENGTH
#define START_WIDTH SQUARE_SIDE_LENGTH
#define START_HEIGHT SQUARE_SIDE_LENGTH


struct SimParams {
    int steps_per_frame;
    int normalize_after_each_step;
    int texel_width, texel_height;
    float t, dt;
    float m, hbar, c, q;
    float width, height, dx, dy;
    struct {
        float amplitude;
        float sigma_x, sigma_y;
        float u0, v0;
        float nx, ny;
        float total_density;
        struct Vec3 spin_direction;
        struct Complex2 spin;
    } init_wavepacket;
    struct {
       int which;
       float x_amplitude;
       float y_amplitude;
    } preset_potential;
};


struct Programs {
    GLuint zero, copy, resize_copy, copy2, scale, complex_multiply;
    GLuint init_dist;
    GLuint add_to_density;
    GLuint kinetic_step;
    GLuint potential_step;
    GLuint current;
    GLuint del_dot_current;
    GLuint preset_potential;
    GLuint preset_magnetic_field;
    GLuint pressure_iter, project_pressure;
    GLuint curl;
    GLuint advection;
    GLuint fft_iter, rearrange, fftshift;
    GLuint dual_view;
    GLuint view;
};


#define SUB_VIEW_ARR_SIZE 2
#define SIM_ARR_SIZE 12
#define FFT_ARR_SIZE 3
#define COLOUR_DENSITY_SIZE 2
#define SUM_ARR_SIZE 8

struct Frames {
    union {
        struct {
            frame_id view;
            frame_id secondary_view1;
            frame_id secondary_view2;
            frame_id wavefunc[2];
            frame_id px, py, p2;
            frame_id wavefunc_p;
            frame_id shifted_wavefunc_p;
            frame_id potential;
            frame_id current;
            frame_id del_dot_current;
            frame_id pressure_iter2[2];
            frame_id bit_sort_table;
            frame_id fft_iter2[2];
            frame_id colour_densities[2];
            frame_id summation[8];
        };
        struct {
            frame_id main_view;
            frame_id sub_views[SUB_VIEW_ARR_SIZE];
            frame_id simulation[SIM_ARR_SIZE];
            frame_id fft[FFT_ARR_SIZE];
            frame_id colour_view_densities[COLOUR_DENSITY_SIZE];
            frame_id sum_iter[SUM_ARR_SIZE];
        };
    };
};


void init_sim_params(struct SimParams *sim_params);

void init_programs(struct Programs *programs);

void init_wavepacket(GLuint init_wavepacket_program,
                     const struct SimParams *params,
                     frame_id frame);

void add_to_colour_density(GLuint add_to_density_program,
                           const struct SimParams *params,
                           frame_id prev, frame_id next,
                           float r, float g, float b);

void init_preset_potential(const struct SimParams *params,
                           const struct Programs *programs,
                           const struct Frames *quads);

void init_frames(struct Frames *frames,
                 const struct SimParams *sim_params);

void set_uniforms_dx_dy_width_height(float dx, float dy,
                                     float width, float height);

void swap3(frame_id *f1, frame_id *f2, frame_id *f3);

frame_id ft(const struct SimParams *params,
            const struct Programs *programs,
            const struct Frames *quads, frame_id initial);

frame_id ift(const struct SimParams *params,
             const struct Programs *programs,
             const struct Frames *quads, frame_id initial);

frame_id timestep(const struct SimParams *params,
                  const struct Programs *programs,
                  const struct Frames *quads);

void compute_current(const struct SimParams *params,
                     const struct Programs *programs,
                     const struct Frames *quads, frame_id wavefunc_tex);

void compute_del_dot_current(const struct SimParams *params,
                             const struct Programs *programs,
                             const struct Frames *quads,
                             frame_id wavefunc_tex);

void normalize_then_scale(struct SimParams *params,
                          const struct Programs *programs,
                          struct Frames *quads, float scale_val);

#endif
#ifdef __cplusplus
}
#endif

