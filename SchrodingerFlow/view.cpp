#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gl_wrappers/gl_wrappers.h"
#include "fft_gl.h"
#include "simulation.h"
#include "summation_gl.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// #include <emscirpten/html5.h>
#endif
#include <functional>


struct Click {
    double x, y;
    double dx, dy;
    int first_press;
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
        click->first_press = (click->pressed == 0)? 1: 0;
        click->pressed = 1;
    } else {
        if (click->released) click->released = 0;
        if (click->pressed) click->released = 1;
        click->pressed = 0;
    }
}

struct ViewParams {
    // float wavefunction_brightness = 4.0;
    float wavefunction_brightness = 1.0;
    float potential_brightness = 10.0;
    int view_mode_1 = 0;
    int view_mode_2 = 0;
    float first_press_x, first_press_y;
} view_params;

struct SimParams sim_params;
struct Programs programs;
struct Frames quads;

void swap2(frame_id *a, frame_id *b) {
    frame_id tmp = *a;
    *a = *b;
    *b = tmp;
}

#ifdef __EMSCRIPTEN__
std::function <void ()> loop;
void main_loop();
void main_loop() {
    loop();
}
#endif

int main() {

    int width = START_WIDTH, height = START_HEIGHT;
    #ifndef __APPLE__
    int view_ratio = 512/width;
    #else
    int view_ratio = 1024/width;
    #endif
    int pixel_width = 2*width*view_ratio;
    int pixel_height = height*view_ratio;

    GLFWwindow *window = init_window(pixel_width, pixel_height);
    init_programs(&programs);
    init_sim_params(&sim_params);
    init_frames(&quads, &sim_params);

    // Initialize imgui
    bool show_controls_window = true;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    #ifndef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 330 core");
    #else
    ImGui_ImplOpenGL3_Init("#version 300 es");
    #endif

    glViewport(0, 0, width, height);
    make_reverse_bit_sort_table(quads.bit_sort_table, width);
    bind_quad(quads.potential, programs.zero);
    draw_unbind();

    int k = 0;
    #ifndef __EMSCRIPTEN__
    auto
    #endif
    loop = [&] {

        glViewport(0, 0, width, height);

        timestep(&sim_params, &programs, &quads);
        swap2(&quads.colour_densities[0], &quads.colour_densities[1]);
        if (sim_params.normalize_after_each_step) {
            float rho = sim_params.init_wavepacket.total_density;
            normalize_then_scale(&sim_params, &programs, &quads, sqrt(rho));
        }
        /*if (view_params.view_mode_1 == 8) {
            compute_current(&sim_params, &programs, &quads, quads.wavefunc[0]);
            }*/
        sim_params.t += sim_params.dt;

        if (left_click.first_press && k > 0) {
            view_params.first_press_x = left_click.x;
            view_params.first_press_y = left_click.y;
            bind_quad(quads.colour_densities[0], programs.zero);
            draw_unbind();
        }
        if (left_click.pressed && k > 0) {
            float u0 = (view_params.first_press_x < 0.5)?
                2.0*view_params.first_press_x:
                2.0*(view_params.first_press_x - 0.5);
            float v0 = view_params.first_press_y;
            sim_params.init_wavepacket.u0 = u0;
            sim_params.init_wavepacket.v0 = v0;
            // printf("%f\n", sim_params.init_wavepacket.u0 );
            // float nx = 200.0*left_click.dx > 0.25*(float)w
            float x = (view_params.first_press_x < 0.5)?
                2.0*left_click.x: 2.0*(left_click.x - 0.5);
            float y = left_click.y;
            float dist_x = x - u0;
            float dist_y = y - v0;
            sim_params.init_wavepacket.nx
                = (dist_x > 0.0)?
                fmin(0.25*width*dist_x, 0.125*(float)width):
                fmax(0.25*width*dist_x, -0.125*(float)width);
            sim_params.init_wavepacket.ny
                = (dist_y > 0.0)?
                fmin(0.25*height*dist_y, 0.125*(float)height):
                fmax(0.25*height*dist_y, -0.125*(float)height);
            float sigma_x = sim_params.init_wavepacket.sigma_x;
            float sigma_y = sim_params.init_wavepacket.sigma_y;
            if (view_params.first_press_x >= 0.5) {
                // sim_params.init_wavepacket.sigma_x = 0.01;
                // sim_params.init_wavepacket.sigma_y = 0.01;
                sim_params.init_wavepacket.sigma_x = 0.05;
                sim_params.init_wavepacket.sigma_y = 0.05;
                sim_params.init_wavepacket.nx *= -4.0;
                sim_params.init_wavepacket.ny *= -4.0;
                sim_params.init_wavepacket.nx -= width*0.5;
                sim_params.init_wavepacket.ny -= height*0.5;
            }
            init_wavepacket(programs.init_dist, &sim_params,
                            (view_params.first_press_x < 0.5)?
                            quads.wavefunc[0]: quads.shifted_wavefunc_p);
            sim_params.init_wavepacket.sigma_x = sigma_x;
            sim_params.init_wavepacket.sigma_y = sigma_y;
        }
        if (left_click.released && k > 0) {
            add_to_colour_density(programs.add_to_density, &sim_params,
                                  quads.colour_densities[0],
                                  quads.colour_densities[1],
                                  1.0, 0.0, 0.0);
            swap2(quads.colour_densities, &quads.colour_densities[1]);
        }
        if (left_click.pressed && k > 0 &&
            view_params.first_press_x >= 0.5) {
            bind_quad(quads.wavefunc_p, programs.scale);
            set_float_uniform("scale", 1000.0);
            set_sampler2D_uniform("tex", quads.shifted_wavefunc_p);
            draw_unbind();
            bind_quad(quads.shifted_wavefunc_p, programs.fftshift);
            set_sampler2D_uniform("tex", quads.wavefunc_p);
            set_int_uniform("isVertical", 0);
            draw_unbind();
            bind_quad(quads.wavefunc_p, programs.fftshift);
            set_sampler2D_uniform("tex", quads.shifted_wavefunc_p);
            set_int_uniform("isVertical", 1);
            draw_unbind();
            frame_id res = ift(&sim_params, &programs, &quads,
                               quads.wavefunc_p);
            bind_quad(quads.wavefunc[0], programs.copy);
            // set_float_uniform("scale", 10000000.0);
            set_sampler2D_uniform("tex", res);
            draw_unbind();
        }

        if (left_click.pressed && k > 0) {
            compute_del_dot_current(&sim_params,
                                    &programs, &quads, quads.wavefunc[0]);
            bind_quad(quads.pressure_iter2[0], programs.copy);
            set_sampler2D_uniform("tex", quads.del_dot_current);
            draw_unbind();
            // float rho = sim_params.init_wavepacket.total_density;
            // normalize_then_scale(&sim_params, &programs, &quads, sqrt(rho));
        }


        bind_quad(quads.secondary_view1, programs.view);
        set_int_uniform("viewMode", view_params.view_mode_1);
        set_float_uniform("wavefuncBrightnessScale1",
                          view_params.wavefunction_brightness);
        set_float_uniform("wavefuncBrightnessScale2",
                          view_params.wavefunction_brightness);
        set_float_uniform("potentialBrightnessScale",
                          view_params.potential_brightness);
        set_sampler2D_uniform("texWavefunc0", quads.wavefunc[0]);
        set_sampler2D_uniform("texWavefunc1", quads.wavefunc[0]);
        set_sampler2D_uniform("texWavefunc2", quads.wavefunc[0]);
        set_sampler2D_uniform("texPotential", quads.potential);
        set_sampler2D_uniform("texCurrent", quads.current);
        set_sampler2D_uniform("texDelDotCurrent", quads.del_dot_current);
        set_sampler2D_uniform("texColourDensity", quads.colour_densities[0]);
        draw_unbind();

        bind_quad(quads.shifted_wavefunc_p, programs.fftshift);
        set_sampler2D_uniform("tex", quads.wavefunc_p);
        set_int_uniform("isVertical", 1);
        draw_unbind();
        bind_quad(quads.wavefunc_p, programs.fftshift);
        set_sampler2D_uniform("tex", quads.shifted_wavefunc_p);
        set_int_uniform("isVertical", 0);
        draw_unbind();
        bind_quad(quads.secondary_view2, programs.view);
        set_int_uniform("viewMode", view_params.view_mode_2);
        set_float_uniform("wavefuncBrightnessScale1",
                          view_params.wavefunction_brightness/4000000.0);
        set_float_uniform("potentialBrightnessScale", 0.0);
        set_sampler2D_uniform("texWavefunc0", quads.wavefunc_p);
        set_sampler2D_uniform("texWavefunc1", quads.wavefunc_p);
        // set_sampler2D_uniform("texWavefunc1", quads.wavefunc_p);
        set_sampler2D_uniform("texWavefunc2", quads.wavefunc_p);
        draw_unbind();

        glViewport(0, 0, pixel_width, pixel_height);
        bind_quad(quads.main_view, programs.dual_view);
        set_sampler2D_uniform("tex1", quads.secondary_view1);
        set_sampler2D_uniform("tex2", quads.secondary_view2);
        // set_float_uniform("xTransform", 2.0);
        // set_float_uniform("yTransform", 1.0);
        // set_vec2_uniform("offset", 0.0, 0.0);
        draw_unbind();

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_controls_window) {
            ImGui::Begin("Controls", &show_controls_window);
            ImGui::Text("WIP AND INCOMPLETE");
            ImGui::SliderInt("View Mode 1", &view_params.view_mode_1, 0, 10);
            enum {DENSITY_PHASE1=0, DENSITY_PHASE2, DENSITY1,
                  DENSITY2, DENSITY_PHASE_ALL, DENSITY_ALL,
                  DENSITY_ALL_DIFF_COLOURS, SPIN, CURRENT, DEL_DOT_CURRENT,
                  COLOUR_DENSITY};
            auto show_view_mode = [&](int view_mode) {
                switch (view_mode) {
                case DENSITY_PHASE1:
                    ImGui::Text("Density phase 1");
                    break;
                case DENSITY_PHASE2:
                    ImGui::Text("Density phase 2");
                    break;
                case DENSITY1:
                    ImGui::Text("Density 1");
                    break;
                case DENSITY2:
                    ImGui::Text("Density 2");
                    break;
                case DENSITY_PHASE_ALL:
                    ImGui::Text("Density phase all");
                    break;
                case DENSITY_ALL:
                    ImGui::Text("Density all");
                    break;
                case DENSITY_ALL_DIFF_COLOURS:
                    ImGui::Text("Density all diff colours");
                    break;
                case SPIN:
                    ImGui::Text("Spin");
                    break;
                case CURRENT:
                    ImGui::Text("Current");
                    break;
                case DEL_DOT_CURRENT:
                    ImGui::Text("del dot current");
                    break;
                case COLOUR_DENSITY:
                    ImGui::Text("colour density");
                    break;
                };
            };
            show_view_mode(view_params.view_mode_1);
            ImGui::SliderInt("View Mode 2", &view_params.view_mode_2, 0, 7);
            show_view_mode(view_params.view_mode_2);
            ImGui::SliderFloat("hbar", &sim_params.hbar, 0.01, 10.0);
            ImGui::SliderFloat("dt", &sim_params.dt, 0.01, 5.0);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (!io.WantCaptureMouse) click_update(&left_click, window);
        glfwSwapBuffers(window);
        #ifdef __EMSCRIPTEN__
        k++;
        #endif
    };

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
    #else
    for (k = 0; !glfwWindowShouldClose(window); k++) {
        loop();
    }
    #endif
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

