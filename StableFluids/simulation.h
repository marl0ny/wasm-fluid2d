#include "gl_wrappers/gl_wrappers.h"

#ifndef _SIMULATION_H_
#define _SIMULATION_H


struct Programs {
    GLuint add_forces;
    GLuint advection;
    GLuint copy;
    GLuint copy2;
    GLuint diffusion_iter;
    GLuint divergence;
    GLuint init_dist;
    GLuint pressure_iter;
    GLuint subtract_gradp;
    GLuint enforce_free_slip;
    GLuint zero;
    GLuint view;
    GLuint barrier_velocity;
};

#define NUMBER_OF_FRAMES 17
struct Frames {
    union {
        struct {
            frame_id view; // Output frame.
            frame_id init_density;
            frame_id densities2[2]; // Primary quantity being visualized.
                                    // These must be swapped manually after
                                    // each time step.
            frame_id fluid_velocity;
            frame_id advection_result;
            frame_id diffusion_iter2[2];
            frame_id diffusion_result;
            frame_id add_forces_result;
            frame_id div_velocity;
            frame_id pressure_iter2[2];
            frame_id pressure_result;
            frame_id force;
            frame_id barrier_velocity;
            frame_id tmps[3];
        };
        struct {
            frame_id all[NUMBER_OF_FRAMES];
        };
    };
};

struct SimParams {
    float dt;
    float dx, dy;
    float width, height; // Actual width and height used for the simulation
    float nu;
    float diff_type;
    int diffusion_iterations;
    int pressure_iterations;
    int boundary_mode;
};

struct DistParams {
    float amplitude;
    float width, height; // Width and height of view screen
    float uc, vc; // Centre of Gaussian-shaped drop,
                  // in normalized texture coordinates
    float sx, sy; // Standard deviations
    float r, g, b; // Colour of drop
};

void init_programs(struct Programs *programs);

void dist_params_set_rgb(struct DistParams *dist_params,
                         float r, float g, float b);

void init_sim_params(struct SimParams *sim_params,
                     int texel_width, int texel_height);

void init_dist_params(struct DistParams *dist_params,
                      int texel_width, int texel_height);

void init_frames(struct Frames *quads, const struct SimParams *sim_params,
                 int texel_width, int texel_height);

void init_dist(GLuint init_dist_program,
               const struct DistParams *params,
               frame_id quad);

void set_uniforms_dx_dy_width_height(float dx, float dy,
                                     float width, float height);

void time_step(const struct SimParams *sim_params,
               const struct DistParams *dist_params,
               const struct Programs *programs,
               const struct Frames *quads);


#endif
