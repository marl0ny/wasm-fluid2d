#include <iostream>
#include <utility>
#ifdef __EMSCRIPTEN__
#include "shaders.hpp"
#include <functional>
#include <emscripten.h>
#endif
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "gl_wrappers2d/gl_wrappers.h"

struct MouseClick {
    double x, y;
    double dx, dy;
    bool pressed = false;
    bool released = false;
    int w, h;
    int mouse_button;
    MouseClick(int mb) {
        mouse_button = mb;
    }
    void update(GLFWwindow *window) {
        double x_prev = x;
        double y_prev = y;
        glfwGetFramebufferSize(window, &w, &h);
        glfwGetCursorPos(window, &x, &y);
        x = x/(double)w;
        y = 1.0 - y/(double)h;
        dx = x - x_prev;
        dy = y - y_prev;
        if (glfwGetMouseButton(window, mouse_button) == GLFW_PRESS) {
            pressed = true;
        } else {
            if (released) released = false;
            if (pressed) released = true;
            pressed = false;
        }
    }
};


void button_update(GLFWwindow *window, int button_key, 
                    int &param, int new_val) {
    if (glfwGetKey(window, button_key) == GLFW_PRESS) {
        param = new_val;
    }
}


#ifdef __EMSCRIPTEN__
std::function<void ()> loop;
void browser_loop();
void browser_loop() {
    loop();
}
#endif

int main() {


    int width = 512, height = 128;
    // width *= 2;
    // height *= 2;
    float view_ratio = 2.5;
    GLFWwindow *window = init_window(view_ratio*width, view_ratio*height);
    init_glew();
    glViewport(0, 0, width, height);
    MouseClick left_click = MouseClick(GLFW_MOUSE_BUTTON_1);
    MouseClick right_click = MouseClick(GLFW_MOUSE_BUTTON_2);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *IMGUI_LICENSE = 
    R"(The MIT License (MIT)

Copyright (c) 2014-2021 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
    )";
    bool show_license = false;

    #ifdef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 100");
    #else
    ImGui_ImplOpenGL3_Init("#version 330");
    #endif
    bool show_controls_window = true;
    bool restart = false;
    bool reset = false;

    #ifdef  __EMSCRIPTEN__
    GLuint add_vel_shader = make_fragment_shader(add_vel_shader_source);
    GLuint copy_over_shader = make_fragment_shader(copy_over_shader_source);
    GLuint density_average_vel_shader = 
        make_fragment_shader(density_average_vel_shader_source);
    GLuint draw_shader = make_fragment_shader(draw_shader_source);
    GLuint init_barrier_shader = 
        make_fragment_shader(init_barrier_shader_source);
    GLuint init_cond_shader = 
        make_fragment_shader(init_cond_shader_source);
    GLuint collisions_down_shader = 
        make_fragment_shader(collisions_down_shader_source);
    GLuint collisions_horizontals_shader = 
        make_fragment_shader(collisions_horizontals_shader_source);
    GLuint collisions_ups_shader = 
        make_fragment_shader(collisions_ups_shader_source);
    GLuint stream_down_shader = 
        make_fragment_shader(stream_down_shader_source);
    GLuint stream_horizontals_shader = 
        make_fragment_shader(stream_horizontals_shader_source);
    GLuint stream_ups_shader = 
        make_fragment_shader(stream_ups_shader_source);
    GLuint vertices_shader = 
        make_vertex_shader(vertices_shader_source);
    GLuint view_shader = 
        make_fragment_shader(view_shader_source);
    #else
    GLuint vertices_shader = get_shader("./shaders/vertices.vert", 
                                        GL_VERTEX_SHADER);
    GLuint density_average_vel_shader = get_shader(
        "./shaders/density-average-vel.frag", GL_FRAGMENT_SHADER);
    GLuint copy_over_shader = get_shader("./shaders/copy-over.frag",
                                         GL_FRAGMENT_SHADER);
    GLuint collisions_down_shader = get_shader(
                                        "./shaders/collisions-down.frag",
                                         GL_FRAGMENT_SHADER);
    GLuint collisions_horizontals_shader = get_shader(
        "./shaders/collisions-horizontals.frag", GL_FRAGMENT_SHADER);
    GLuint collisions_ups_shader = get_shader("./shaders/collisions-ups.frag",
                                        GL_FRAGMENT_SHADER);
    GLuint stream_down_shader = get_shader("./shaders/stream-down.frag",
                                           GL_FRAGMENT_SHADER);
    GLuint stream_horizontals_shader = get_shader(
        "./shaders/stream-horizontals.frag", GL_FRAGMENT_SHADER);
    GLuint stream_ups_shader = get_shader(
        "./shaders/stream-ups.frag", GL_FRAGMENT_SHADER);
    GLuint init_cond_shader = get_shader("./shaders/init-cond.frag",
                                         GL_FRAGMENT_SHADER);
    GLuint draw_shader = get_shader("./shaders/draw.frag", 
                                    GL_FRAGMENT_SHADER);
    GLuint view_shader = get_shader("./shaders/view.frag",
                                    GL_FRAGMENT_SHADER);
    GLuint add_vel_shader = get_shader("./shaders/add-vel.frag",
                                       GL_FRAGMENT_SHADER);
    GLuint init_barrier_shader = get_shader("./shaders/init-barrier.frag",
                                            GL_FRAGMENT_SHADER);
    #endif

    GLuint density_program = make_program(
        vertices_shader, density_average_vel_shader);
    GLuint copy_program = make_program(vertices_shader, copy_over_shader);
    GLuint collisions_down_program = make_program(vertices_shader, 
                                                  collisions_down_shader);
    GLuint collisions_horizontals_program = make_program(
        vertices_shader, collisions_horizontals_shader);
    GLuint collision_ups_program = make_program(vertices_shader, 
                                              collisions_ups_shader);
    GLuint stream_down_program = make_program(vertices_shader, 
                                              stream_down_shader);
    GLuint stream_h_program = make_program(
        vertices_shader, stream_horizontals_shader);
    GLuint stream_ups_program = make_program(vertices_shader, 
                                             stream_ups_shader);
    GLuint init_cond_program = make_program(vertices_shader, 
                                            init_cond_shader);
    GLuint draw_program = make_program(vertices_shader, draw_shader);
    GLuint view_program = make_program(vertices_shader, view_shader);
    GLuint add_vel_program = make_program(vertices_shader, add_vel_shader);
    GLuint init_barrier_program = make_program(vertices_shader, 
                                              init_barrier_shader);

    /* List all programs (separated by comma and newline):
    mapfile -t f < <(egrep fluid2d.cpp -oh -e "[a-zA-Z_]+_program[ ]")
    for e in ${f[@]}; do echo "$e,"; done
    Likewise for the shaders:
    mapfile -t f < <(egrep fluid2d.cpp -oh -e "[a-zA-Z_]+_shader[ ]")
    for e in ${f[@]}; do echo "$e,"; done */
    auto shaders = {
        vertices_shader,
        density_average_vel_shader,
        copy_over_shader,
        collisions_down_shader,
        collisions_horizontals_shader,
        collisions_ups_shader,
        stream_down_shader,
        stream_horizontals_shader,
        stream_ups_shader,
        init_cond_shader,
        draw_shader,
        view_shader,
    };
    for (auto s: shaders) {
        glDeleteShader(s);
    }
    auto programs = {
        copy_program,
        density_program,
        collisions_down_program,
        collisions_horizontals_program,
        collision_ups_program,
        stream_down_program,
        stream_h_program,
        stream_ups_program,
        init_cond_program,
        draw_program,
        view_program,
    };
    Quad view_frame = Quad::make_frame(view_ratio*width, view_ratio*height);
    Quad draw_frame = Quad::make_float_frame(width, height);
    Quad density_vel_frame = Quad::make_float_frame(width, height);
    Quad barrier = Quad::make_float_frame(width, height);
    // d -> Store the lower left, downwards, and lower right quantities.
    // c -> Store the left, centre, and right quantities.
    // u -> Store the upper left, upwards, and upper right quantities.
    Quad d1 = Quad::make_float_frame(width, height);
    Quad c1 = Quad::make_float_frame(width, height);
    Quad u1 = Quad::make_float_frame(width, height);
    Quad d2 = Quad::make_float_frame(width, height);
    Quad c2 = Quad::make_float_frame(width, height);
    Quad u2 = Quad::make_float_frame(width, height);
    Quad *quads[6] = {&d1, &c1, &u1, &d2, &c2, &u2};

    int view_mode = 0;
    float density_contrast = 1.0;
    float brightness = 1.0;
    enum MouseMode{DRAW_BARRIER, DRAW_FLOW};
    int mouse_mode = DRAW_BARRIER;
    enum BarrierPresets{NONE, UP_DOWN_BOUNDARY, HORIZONTAL_OVAL, 
                        VERTICAL_RECTANGLE, CIRCLE};
    int barrier_preset = VERTICAL_RECTANGLE;

    // float init_vel = 0.375;
    float init_vel = 0.30;
    auto reset_fluid = [&] {
        quads[1]->bind(init_cond_program);
        quads[1]->set_float_uniforms(
            // 1.6 : 0.64
            {{"left", 0.0}, {"centre", 1.6}, {"right", 1.6*init_vel},
             {"dx", 1.0/(float)width}}
            );
        quads[1]->set_int_uniform("barrierTex", barrier.get_value());
        quads[1]->draw();
        unbind();
        quads[4]->bind(init_cond_program);
        quads[4]->set_float_uniforms(
            {{"left", 0.0}, {"centre", 1.6}, {"right", 1.6*init_vel},
             {"dx", 1.0/(float)width}}
            );
        quads[4]->set_int_uniform("barrierTex", barrier.get_value());
        quads[4]->draw();
        unbind();
        auto nums = {0, 2, 3, 5};
        for (auto &i: nums) {
            quads[i]->bind(init_cond_program);
            quads[i]->set_float_uniforms(
                {{"left", 0.0}, {"centre", 0.0}, {"right", 0.0},
                {"dx", 1.0/(float)width}}
                );
            quads[i]->set_int_uniform("barrierTex", barrier.get_value());
            quads[i]->draw();
            unbind();
        }
    };

    auto set_init_cond = [&] {
        barrier.bind(init_barrier_program);
        barrier.set_float_uniform("dy", 1.0/(float)height);
        barrier.set_float_uniform("dx", 1.0/(float)width);
        barrier.set_int_uniform("barrierType", barrier_preset);
        barrier.draw();
        unbind();
        reset_fluid();
    };

    auto draw_barrier = [&] {
        auto scale_click_type = [&](double click_type) {
            return click_type;};
        double w_stencil = 0.025;
        double h_stencil = 0.025;
        left_click.x = scale_click_type(left_click.x);
        left_click.y = scale_click_type(left_click.y);
        right_click.x = scale_click_type(right_click.x);
        right_click.y = scale_click_type(right_click.y);
        if (left_click.pressed) {
            for (int i = 0; i < 6; i++) {
                draw_frame.bind(draw_program);
                draw_frame.set_int_uniform("tex", quads[i]->get_value());
                draw_frame.set_float_uniforms({
                    {"xc", left_click.x}, {"yc", left_click.y},
                    {"h", h_stencil}, {"w", w_stencil*((float)height/width)},
                    {"r", 0.0}, {"g", 0.0}, {"b", 0.0}
                });
                draw_frame.draw();
                unbind();
                quads[i]->bind(copy_program);
                quads[i]->set_int_uniform("tex", draw_frame.get_value());
                quads[i]->draw();
                unbind();
            }
        }
        draw_frame.bind(draw_program);
        draw_frame.set_int_uniform("tex", barrier.get_value());
        draw_frame.set_float_uniforms({
            {"xc", (right_click.pressed)? right_click.x: left_click.x}, 
            {"yc", (right_click.pressed)? right_click.y: left_click.y},
            {"h", h_stencil}, {"w", w_stencil*((float)height/width)},
            {"r", (right_click.pressed)? 0.0: 1.0}, 
            {"g", 0.0}, {"b", 0.0}
        });
        draw_frame.draw();
        unbind();
        barrier.bind(copy_program);
        barrier.set_int_uniform("tex", draw_frame.get_value());
        barrier.draw();
        unbind();
    };

    auto draw_fluid = [&] {
        double w_stencil = 0.025;
        double h_stencil = 0.025;
        double invsqrt2 = 0.7071067811865475;
        double ur[2] = {invsqrt2, invsqrt2};
        double ul[2] = {invsqrt2, -invsqrt2};
        auto max = [](float a, float b) {return (a > b)? a: b;};
        if (left_click.pressed) {
            float dx = left_click.dx, dy = left_click.dy;
            float centre_val = 0.25;
            float left_val = 2.0*max(-dx, 0.0);
            float right_val = 2.0*max(dx, 0.0);
            float up_val = 2.0*max(dy, 0.0);
            float down_val = 2.0*max(-dy, 0.0);
            float up_right = ur[0]*dy + ur[1]*dx;
            float up_right_val = 2.0*max(up_right, 0.0);
            float down_left_val = 2.0*max(-up_right, 0.0);
            float up_left = ul[0]*dy + ul[1]*dx;
            float up_left_val = 2.0*max(up_left, 0.0);
            float down_right_val = 2.0*max(-up_left, 0.0);
            for (int i = 0; i < 6; i++) {
                draw_frame.bind(add_vel_program);
                draw_frame.set_int_uniform("tex", quads[i]->get_value());
                float left = 0.0, right = 0.0, centre = 0.0;
                if (i == 0 || i == 3) {
                    centre = down_val;
                    left = down_left_val;
                    right = down_right_val;
                } else if (i == 1 || i == 4) {
                    centre = centre_val;
                    left = left_val;
                    right = right_val;
                } else {
                    centre = up_val;
                    left = up_left_val;
                    right = up_right_val;
                }
                draw_frame.set_float_uniforms({
                    {"xc", left_click.x}, {"yc", left_click.y},
                    {"sy", h_stencil}, 
                    {"sx", w_stencil*((float)height/width)},
                    {"left", left}, {"right", right}, 
                    {"centre", centre} 
                });
                draw_frame.draw();
                unbind();
                quads[i]->bind(copy_program);
                quads[i]->set_int_uniform("tex", draw_frame.get_value());
                quads[i]->draw();
                unbind();
            }
        }
    };

    auto draw = [&] {
        if (mouse_mode == DRAW_BARRIER) {
            draw_barrier();
        } else if (mouse_mode == DRAW_FLOW) {
            draw_fluid();
        }
    };

    int down1=0, horiz1=1, ups1=2;
    int down2=3, horiz2=4, ups2=5;
    double omega = 1.1;

    auto density_mean_vel = [&]{
        density_vel_frame.bind(density_program);
        density_vel_frame.set_int_uniforms({
            {"upTex", quads[ups1]->get_value()},
            {"leftCenterRightTex", quads[horiz1]->get_value()},
            {"downTex", quads[down1]->get_value()}});
        density_vel_frame.draw();
        unbind();
    };

    auto collisions = [&]{

        quads[down2]->bind(collisions_down_program);
        quads[down2]->set_int_uniforms({
            {"downTex", quads[down1]->get_value()},
            {"densityVelTex", density_vel_frame.get_value()}
        });
        quads[down2]->set_float_uniforms({
            {"omega", omega},
        });
        quads[down2]->draw();
        unbind();

        quads[horiz2]->bind(collisions_horizontals_program);
        quads[horiz2]->set_int_uniforms({
            {"leftCenterRightTex", quads[horiz1]->get_value()},
            {"densityVelTex", density_vel_frame.get_value()}
        });
        quads[horiz2]->set_float_uniforms({
            {"omega", omega},
        });
        quads[horiz2]->draw();
        unbind();

        quads[ups2]->bind(collision_ups_program);
        quads[ups2]->set_int_uniforms({
            {"upTex", quads[ups1]->get_value()},
            {"densityVelTex", density_vel_frame.get_value()}
        });
        quads[ups2]->set_float_uniforms({
            {"omega", omega},
        });
        quads[ups2]->draw();
        unbind();

        std::swap(down1, down2);
        std::swap(horiz1, horiz2);
        std::swap(ups1, ups2);
    };

    auto streams = [&] {
        quads[down2]->bind(stream_down_program);
        quads[down2]->set_int_uniforms({
            {"upsTex", quads[ups1]->get_value()},
            {"downTex", quads[down1]->get_value()},
            {"barrierTex", barrier.get_value()},
        });
        quads[down2]->set_float_uniforms({
            {"dx", 1.0/width},
            {"dy", 1.0/height}
        });
        quads[down2]->draw();
        unbind();

        quads[horiz2]->bind(stream_h_program);
        quads[horiz2]->set_int_uniforms({
            {"centreTex", quads[horiz1]->get_value()},
            {"barrierTex", barrier.get_value()},
        });
        quads[horiz2]->set_float_uniforms({
            {"dx", 1.0/width},
            {"dy", 1.0/height},
            {"srcLeft", 0.0}, {"srcCentre", 1.6}, {"srcRight", 1.6*init_vel}
        });
        quads[horiz2]->draw();
        unbind();

        quads[ups2]->bind(stream_ups_program);
        quads[ups2]->set_int_uniforms({
            {"upsTex", quads[ups1]->get_value()},
            {"downTex", quads[down1]->get_value()},
            {"barrierTex", barrier.get_value()},
        });
        quads[ups2]->set_float_uniforms({
            {"dx", 1.0/width},
            {"dy", 1.0/height}
        });
        quads[ups2]->draw();
        unbind();

        std::swap(down1, down2);
        std::swap(horiz1, horiz2);
        std::swap(ups1, ups2);
    };

    /*auto add_vel = [&] {
        quads[horiz2]->bind(add_vel_program);
        quads[horiz2]->set_int_uniform("tex", quads[horiz1]->get_value());
        quads[horiz2]->set_float_uniforms({
            {"left", 0.0}, {"centre", 1.6}, {"right", 0.64}, {"dx", 2.0/width}
        });
        quads[horiz2]->draw();
        unbind();
        std::swap(horiz1, horiz2);

    };*/

    int steps_per_frame = 3;
    #ifndef __EMSCRIPTEN__
    auto
    #endif
    loop = [&] {
        if (restart || reset) {
            for (auto &q: quads) {
                q->bind();
                q->set_program(copy_program);
                q->set_int_uniform("tex", Quad::get_blank());
                q->draw();
                unbind();
            }
            if (reset) reset_fluid();
            if (restart) set_init_cond();
            restart = false;
            reset = false;
        }
        if (left_click.pressed || right_click.pressed) {
            draw();
        }
        for (int _ = 0; _ < steps_per_frame; _++) {
            density_mean_vel();
            collisions();
            streams();
        }
        // glViewport needs to be called whenever switching
        // to framebuffers of different sizes:
        // https://stackoverflow.com/a/33719126
        // answer by datenwolf (https://stackoverflow.com/users/524368)
        // question (https://stackoverflow.com/q/33718237)
        // by Goldboa (https://stackoverflow.com/users/4567996) with
        // edits by Reto Koradi (https://stackoverflow.com/users/3530129).
        glViewport(0, 0, view_ratio*width, view_ratio*height); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        view_frame.bind(view_program);
        view_frame.set_float_uniforms({{"dx", 1.0/width}, 
                                       {"dy", 1.0/height}});
        view_frame.set_float_uniform("contrast", density_contrast);
        view_frame.set_float_uniform("brightness", brightness);
        view_frame.set_int_uniform("barrierTex", barrier.get_value());
        view_frame.set_int_uniform("upsTex", quads[ups1]->get_value());
        view_frame.set_int_uniform("horizontalsTex", 
                                   quads[horiz1]->get_value());
        view_frame.set_int_uniform("densityVelTex", density_vel_frame.get_value());
        view_frame.set_int_uniform("downTex", quads[down1]->get_value());
        view_frame.set_int_uniform("viewMode", view_mode);
        view_frame.draw();
        unbind();
        glfwPollEvents();
        button_update(window, GLFW_KEY_0, view_mode, 0);
        button_update(window, GLFW_KEY_1, view_mode, 1);
        button_update(window, GLFW_KEY_2, view_mode, 2);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        float viscosity = (1.0/3.0)*(1.0/omega - 0.5);
        if (show_controls_window) {
            ImGui::Begin("Controls", &show_controls_window);
            if (ImGui::Button("Reset Fluid")) reset = true;
            ImGui::SliderInt("Steps/frame",&steps_per_frame, 0, 10);
            omega = 1.0/(3.0*viscosity + 0.5);
            ImGui::SliderFloat("Viscosity", &viscosity, 0.05, 0.3);
            ImGui::SliderFloat("Velocity", &init_vel, 0.0, 0.4);
            ImGui::Selectable("Barrier Options");
            const char *barrier_options[5] = {
                "None", "Up/Down Barrier", "Horizontal Oval", 
                "Vertical Rectangle", "Circle"
            };
            ImGui::ListBox("Barrier", &barrier_preset, barrier_options, 5);
            if (ImGui::Button("Select Barrier")) restart = true;
            const char *visualization_options[3] = {
                "Curl", "Density", "Colour velocity"
            };
            // ImGui::Selectable("Mouse Options");
            const char *mouse_options[2] = {
                "Draw/Erase Barrier", "Perturb Fluid"
            };
            ImGui::Selectable("Mouse Options");
            ImGui::ListBox("Mouse", &mouse_mode, mouse_options, 2);
            ImGui::Selectable("Visualization Options");
            ImGui::ListBox("Vis.", &view_mode, visualization_options, 3);
            ImGui::SliderFloat("Contrast", &density_contrast, 0.1, 2.0);
            ImGui::SliderFloat("Brightness", &brightness, 0.1, 2.0);
            if (ImGui::Button("About the GUI")) show_license = !show_license;
            if (show_license) {
                ImGui::Text("This controls GUI is implemented using Dear ImGui.\n");
                ImGui::Text("%s", IMGUI_LICENSE);
            }
            if (ImGui::Button("Close")) show_controls_window = false;
            ImGui::End();
        }
        omega = 1.0/(3.0*(double)viscosity + 0.5);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (!io.WantCaptureMouse) {
            left_click.update(window);
            right_click.update(window);
        }
        glViewport(0, 0, width, height);
        glfwSwapBuffers(window);
    };

    set_init_cond();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(browser_loop, 0, true);
    #else
    while(!glfwWindowShouldClose(window)) {
        loop();
    }
    #endif

    for (auto &program: programs) {
        glDeleteProgram(program);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
