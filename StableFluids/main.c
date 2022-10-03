#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gl_wrappers.h"
#include "gl_wrappers/gl_wrappers.h"


struct Click {
    double x, y;
    double dx, dy;
    int pressed;
    int released;
    int w, h;
} left_click;

void click_update(struct Click *click, GLFWwindow *window) {
    double prev_x = click->x;
    double prev_y = click->y;
    glfwGetFramebufferSize(window, &click->w, &click->h);
    glfwGetCursorPos(window, &click->x, &click->y);
    #ifndef __APPLE__
    click->x = click->x/(double)click->w;
    click->y = 1.0 - click->y/(double)click->h;
    #else
    click->x = 2.0*click->x/(double)click->w;
    click->y = 1.0 - 2.0*click->y/(double)click->h;
    #endif
    click->dx = click->x - prev_x;
    click->dy = click->y - prev_y;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        click->pressed = 1;
    } else {
        if (click->released) click->released = 0;
        if (click->pressed) click->released = 1;
        click->pressed = 0;
    }
}

void swap_frames(frame_id *f1, frame_id *f2) {
    frame_id tmp = *f1;
    *f1 = *f2;
    *f2 = tmp;
}

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
    GLuint zero;
    GLuint view;
} programs;

struct Frames {
    union {
        struct {
            frame_id view; // Output frame.
            frame_id init_density;
            frame_id densities2[2]; // Primary quantity being visualized.
                                    // These must be swapped manually after
                                    // each time step.
            frame_id start_finish; // Velocity of the fluid before and after.
            frame_id post_advection;
            frame_id diffusion_iter2[2];
            frame_id post_diffusion;
            frame_id post_add_forces;
            frame_id div_velocity;
            frame_id pressure_iter2[2];
            frame_id post_pressure;
            frame_id force;
        };
        struct {
            frame_id all[15];
        };
    };
} quads;

struct SimParams {
    float dt;
    float dx, dy;
    float width, height;
    float nu;
    float diff_type;
    int diffusion_iterations;
    int pressure_iterations;
} sim_params;

struct DistParams {
    float amplitude;
    float width, height;
    float uc, vc;
    float sx, sy;
    float r, g, b;
} dist_params;

void dist_params_set_rgb(struct DistParams *dist_params,
                         float r, float g, float b) {
    dist_params->r = r;
    dist_params->b = b;
    dist_params->g = g;
}

void key_modify_params(GLFWwindow *window,
                       struct DistParams *dist_params,
                       struct SimParams *sim_params) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        dist_params->amplitude *= 1.1;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        dist_params->amplitude *= 0.9;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        dist_params->sx *= 0.9;
        dist_params->sy *= 0.9;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        dist_params->sx *= 1.1;
        dist_params->sy *= 1.1;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, 0.1, 0.0, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, 0.0, 0.1, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, 0.0, 0.0, 0.1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, 0.0, 0.0, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, -0.1, -0.1, -0.1);
    }
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
    draw_unbind();
}

void set_uniforms_dx_dy_width_height(float dx, float dy,
                                     float width, float height) {
    set_float_uniform("dx", dx);
    set_float_uniform("dy", dy);
    set_float_uniform("width", width);
    set_float_uniform("height", height);
}

void time_step(const struct SimParams *sim_params,
               const struct DistParams *dist_params,
               const struct Programs *programs,
               const struct Frames *quads) {
    frame_id iter_quads[2] = {0.0,};
    // Advection
    bind_quad(quads->densities2[1], programs->advection);
    set_float_uniform("dt", sim_params->dt);
    set_int_uniform("velocity1Tex", quads->start_finish);
    set_int_uniform("velocity2Tex", quads->densities2[0]);
    draw_unbind();
    bind_quad(quads->post_advection, programs->advection);
    set_float_uniform("dt", sim_params->dt);
    set_int_uniform("velocity1Tex", quads->start_finish);
    set_int_uniform("velocity2Tex", quads->start_finish);
    draw_unbind();
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
            set_sampler2D_uniform("bTex", quads->post_advection);
            set_sampler2D_uniform("lastIterTex", iter_quads[0]);
            draw_unbind();
            swap_frames(&iter_quads[0], &iter_quads[1]);
        }
        bind_quad(quads->post_diffusion, programs->copy);
        set_sampler2D_uniform("tex", iter_quads[0]);
        draw_unbind();
    }
    frame_id pre_add_forces = (sim_params->diffusion_iterations > 0)?
        quads->post_diffusion: quads->post_advection;
    // Add forces
    bind_quad(quads->post_add_forces, programs->add_forces);
    set_float_uniform("dt", sim_params->dt);
    // set_float_uniform("fdgdfgdf", 0.5465);
    set_sampler2D_uniform("forceTex", quads->force);
    set_sampler2D_uniform("uTex", pre_add_forces);
    draw_unbind();
    // Divergence of fluid velocity
    bind_quad(quads->div_velocity, programs->divergence);
    set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                    sim_params->width, sim_params->height);
    set_int_uniform("diffType", sim_params->diff_type);
    set_sampler2D_uniform("vectTex", quads->post_add_forces);
    draw_unbind();
    // Compute pressure
    iter_quads[0] = quads->pressure_iter2[0];
    iter_quads[1] = quads->pressure_iter2[1];
    for (size_t i = 0; i < sim_params->pressure_iterations; i++) {
        bind_quad(iter_quads[1], programs->pressure_iter);
        set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                        sim_params->width, sim_params->height);
        set_sampler2D_uniform("bTex", quads->div_velocity);
        set_sampler2D_uniform("lastIterTex", iter_quads[0]);
        draw_unbind();
        swap_frames(&iter_quads[0], &iter_quads[1]);
    }
    // Subtract the gradient of the pressure
    bind_quad(quads->start_finish, programs->subtract_gradp);
    set_uniforms_dx_dy_width_height(sim_params->dx, sim_params->dy,
                                    sim_params->width, sim_params->height);
    set_sampler2D_uniform("pressureTex", iter_quads[0]);
    set_sampler2D_uniform("uTex", quads->post_add_forces);
    draw_unbind();
}

struct Vec4 arr[1024*1024];

int main() {

    int width = 256, height = 256;
    #ifndef __APPLE__
    int view_ratio = 2;
    #else
    int view_ratio = 4;
    #endif
    int pixel_width = view_ratio*width;
    int pixel_height = view_ratio*height;
    GLFWwindow *window = init_window(pixel_width, pixel_height);
    programs.add_forces = make_program("./shaders/add-forces.frag");
    programs.advection = make_program("./shaders/advection.frag");
    programs.copy = make_program("./shaders/copy.frag");
    programs.copy2 = make_program( "./shaders/copy2.frag");
    programs.diffusion_iter
        = make_program("./shaders/diffusion-iter.frag");
    programs.divergence = make_program( "./shaders/divergence.frag");
    programs.init_dist = make_program("./shaders/init-dist.frag");
    programs.pressure_iter
        = make_program("./shaders/pressure-iter.frag");
    programs.subtract_gradp
        = make_program("./shaders/subtract-gradp.frag");
    programs.zero = make_program("./shaders/zero.frag");
    programs.view = make_program("./shaders/view.frag");

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float y = i/(float)height;
            float x = j/(float)width;
            float sx=0.1, sy = 0.1;
            float val = exp(-0.5*powf(x-0.5, 2)/(sx*sx)
                            -0.5*powf(y-0.5, 2)/(sy*sy));
            arr[i*width + j].r = 100.0*val;
            arr[i*width + j].g = 0.0;
            arr[i*width + j].b = 0.0;
            arr[i*width + j].a = 1.0;
        }
    }

    struct TextureParams texture_params = {
        .type=GL_FLOAT,
        .width=width, .height=height,
        .generate_mipmap=1,
        .wrap_s=GL_REPEAT, .wrap_t=GL_REPEAT,
        .min_filter=GL_LINEAR, .mag_filter=GL_LINEAR,
    };
    quads.all[0] = new_quad(NULL);
    for (size_t i = 1; i < sizeof(quads); i++) {
        quads.all[i] = new_quad(&texture_params);
    }
    bind_quad(quads.densities2[0], programs.zero);
    draw_unbind();
    bind_quad(quads.densities2[1], programs.zero);
    draw_unbind();
    substitute_array(quads.densities2[0],
                     width, height, GL_FLOAT, (void *)arr);
    substitute_array(quads.densities2[1],
                     width, height, GL_FLOAT, (void *)arr);
    sim_params.dt = 0.1;
    sim_params.height = 1.0;
    sim_params.width = sim_params.height*(float)width/(float)height;
    sim_params.dx = sim_params.width/width;
    sim_params.dy = sim_params.height/height;
    sim_params.nu = 0.00005;

    sim_params.diff_type = 0;
    sim_params.diffusion_iterations = 10;
    sim_params.pressure_iterations = 15;

    dist_params.sy = 0.015;
    dist_params.sx = dist_params.sy*(float)height/(float)width;
    dist_params.r = 0.1;
    dist_params.g = 0.0;
    dist_params.b = 0.0;
    dist_params.amplitude = 16.0;

    for (int k = 0; !glfwWindowShouldClose(window); k++) {
        if (left_click.pressed && k >= 0) {
            float r = dist_params.r, g = dist_params.g, b = dist_params.b;
            dist_params.r = left_click.dx;
            dist_params.g = left_click.dy;
            dist_params.b = 1.0;
            dist_params.uc = left_click.x;
            dist_params.vc = left_click.y;
            dist_params.width = (float)pixel_width;
            dist_params.height = (float)pixel_height;
            init_dist(programs.init_dist,
                      &dist_params, quads.force);
            dist_params.r = r;
            dist_params.g = g;
            dist_params.b = b;
            init_dist(programs.init_dist, &dist_params,
                      quads.init_density);
            bind_quad(quads.densities2[1], programs.copy2);
            set_sampler2D_uniform("tex1", quads.init_density);
            set_sampler2D_uniform("tex2", quads.densities2[0]);
            swap_frames(&quads.densities2[0], &quads.densities2[1]);
            draw_unbind();
        } else {
            bind_quad(quads.force, programs.zero);
            draw_unbind();
        }

        time_step(&sim_params, &dist_params, &programs, &quads);
        swap_frames(&quads.densities2[0], &quads.densities2[1]);

        glViewport(0, 0, pixel_width, pixel_height);
        bind_quad(quads.view, programs.view);
        set_sampler2D_uniform("texCol", quads.densities2[0]);
        set_sampler2D_uniform("texVel", quads.start_finish);
        draw_unbind();
        glViewport(0, 0, width, height);

        key_modify_params(window, &dist_params, &sim_params);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            substitute_array(quads.densities2[0],
                             width, height, GL_FLOAT, (void *)arr);
            substitute_array(quads.densities2[1],
                             width, height, GL_FLOAT, (void *)arr);

        }

        glfwPollEvents();
        click_update(&left_click, window);
        // glViewport(0, 0, width, height);
        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

