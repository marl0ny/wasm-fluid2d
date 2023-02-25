#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gl_wrappers.h"
#include "gl_wrappers/gl_wrappers.h"
#include "simulation.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif


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
    #ifdef __APPLE__
    click->x = 2.0*click->x/(double)click->w;
    click->y = 1.0 - 2.0*click->y/(double)click->h;
    #else
    click->x = click->x/(double)click->w;
    click->y = 1.0 - click->y/(double)click->h;
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
        dist_params_set_rgb(dist_params, 0.01, 0.0, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, 0.0, 0.01, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, 0.0, 0.0, 0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, 0.0, 0.0, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        dist_params_set_rgb(dist_params, -0.1, -0.1, -0.1);
    }
}

struct Programs programs = {};
struct Frames quads = {};
struct SimParams sim_params = {};
struct DistParams dist_params = {};
struct Vec4 arr[1024*1024];

struct MainLoopData {
    struct SimParams *sim_params;
    struct DistParams *dist_params;
    struct Programs *programs;
    struct Frames *quads;
    GLFWwindow *window;
    struct Click *left_click;
    int iter;
    int sim_width, sim_height;
    int view_width, view_height;
} data;

void main_loop() {
    struct SimParams *sim_params = data.sim_params;
    struct DistParams *dist_params = data.dist_params;
    struct Programs *programs = data.programs;
    struct Frames *quads = data.quads;
    GLFWwindow *window = data.window;
    struct Click *left_click = data.left_click;
    int iter = data.iter;
    int sim_width = data.sim_width, sim_height = data.sim_height;
    int view_width = data.view_width, view_height = data.view_height;
    int k = iter;
    if (left_click->pressed && k >= 0) {
        float r = dist_params->r, g = dist_params->g, b = dist_params->b;
        dist_params->r = left_click->dx;
        dist_params->g = left_click->dy;
        dist_params->b = 1.0;
        dist_params->uc = left_click->x;
        dist_params->vc = left_click->y;
        dist_params->width = (float)view_width;
        dist_params->height = (float)view_height;
        init_dist(programs->init_dist,
                  dist_params, quads->force);
        dist_params->r = r;
        dist_params->g = g;
        dist_params->b = b;
        init_dist(programs->init_dist, dist_params,
                    quads->init_density);
        bind_quad(quads->densities2[1], programs->copy2);
        set_sampler2D_uniform("tex1", quads->init_density);
        set_sampler2D_uniform("tex2", quads->densities2[0]);
        swap_frames(&quads->densities2[0], &quads->densities2[1]);
        draw_unbind_quad();
    } else {
        bind_quad(quads->force, programs->zero);
        draw_unbind_quad();
    }

    for (int j = 0; j < 2; j++) {
        time_step(sim_params, dist_params, programs, quads);
        swap_frames(&quads->densities2[0], &quads->densities2[1]);
    }

    glViewport(0, 0, view_width, view_height);
    bind_quad(quads->view, programs->view);
    set_sampler2D_uniform("texCol", quads->densities2[0]);
    set_sampler2D_uniform("texVel", quads->fluid_velocity);
    draw_unbind_quad();
    glViewport(0, 0, sim_width, sim_height);

    key_modify_params(window, dist_params, sim_params);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        quad_substitute_array(quads->densities2[0],
                              sim_width, sim_height, GL_FLOAT, (void *)arr);
        quad_substitute_array(quads->densities2[1],
                              sim_width, sim_height, GL_FLOAT, (void *)arr);

    }
    glfwPollEvents();
    click_update(left_click, window);
    glfwSwapBuffers(window);
    #ifdef __EMSCRIPTEN__
    data.iter++;
    #endif
}


int main() {

    int sim_width = 512, sim_height = 512;
    #ifdef __APPLE__
    int view_ratio = (int)(1024/sim_width);
    #else
    int view_ratio = (int)(512/sim_width);
    #endif
    int view_width = view_ratio*sim_width;
    int view_height = view_ratio*sim_height;

    GLFWwindow *window = init_window(view_width, view_height);
    init_programs(&programs);

    init_sim_params(&sim_params, sim_width, sim_height);
    init_dist_params(&dist_params, sim_width, sim_height);
    init_frames(&quads, &sim_params, sim_width, sim_height);

    /* for (int i = 0; i < sim_height; i++) {
        for (int j = 0; j < sim_width; j++) {
            float y = i/(float)sim_height;
            float x = j/(float)sim_width;
            float sx=0.1, sy = 0.1;
            float val = exp(-0.5*powf(x-0.5, 2)/(sx*sx)
                            -0.5*powf(y-0.5, 2)/(sy*sy));
            arr[i*sim_width + j].r = 100.0*val;
            arr[i*sim_width + j].g = 0.0;
            arr[i*sim_width + j].b = 0.0;
            arr[i*sim_width + j].a = 1.0;
        }
    }
    bind_quad(quads.densities2[0], programs.zero);
    draw_unbind_quad();
    bind_quad(quads.densities2[1], programs.zero);
    draw_unbind_quad();
    quad_substitute_array(quads.densities2[0],
                          sim_width, sim_height, GL_FLOAT, (void *)arr);
    quad_substitute_array(quads.densities2[1],
    sim_width, sim_height, GL_FLOAT, (void *)arr);*/
    // init_dist(programs.init_dist, &dist_params, quads.init_density);
    // init_dist(programs.init_dist, &dist_params, quads.densities2[0]);
    // init_dist(programs.init_dist, &dist_params, quads.densities2[1]);

    data.sim_params = &sim_params;
    data.dist_params = &dist_params;
    data.programs = &programs;
    data.quads = &quads;
    data.window = window;
    data.left_click = &left_click;
    data.iter = 0;
    data.sim_width = sim_width;
    data.sim_height = sim_height;
    data.view_width = view_width;
    data.view_height = view_height;

    #ifdef __EMSCRIPTEN__
    data.iter++;
    emscripten_set_main_loop(main_loop, 0, 1);
    #else
    for (int k = 0; !glfwWindowShouldClose(window); k++) {
        data.iter = k;
        main_loop();
    }
    #endif
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

