#include <iostream>
#ifdef __EMSCRIPTEN__
#include <functional>
#include <emscripten.h>
#endif
#include "gl_wrappers2d/gl_wrappers.h"


struct mouse {
    double x, y;
    int pressed = 0;
    int released = 0;
    int w, h;
    void update(GLFWwindow *window) {
        glfwGetFramebufferSize(window, &w, &h);
        glfwGetCursorPos(window, &x, &y);
        x = x/(double)w;
        y = 1.0 - y/(double)h;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            pressed = 1;
        } else {
            if (pressed) {
                released = 1;
            }
            pressed = 0;
        }
    }
}left_click;

template <typename T>
void swap(T &a, T &b) {
    T t = a;
    a = b;
    b = t;
}


#ifdef __EMSCRIPTEN__
std::function<void ()> loop;
void browser_loop();
void browser_loop() {
    loop();
}
#endif

int main() {

    int width = 512, height = 512;
    GLFWwindow *window = init_window(width, height);
    init_glew();

    #ifdef  __EMSCRIPTEN__
    #include "shaders.h"
    #else
    GLuint vertices_shader = get_shader("./shaders/vertices.vert", GL_VERTEX_SHADER);
    GLuint density_average_vel_shader = get_shader(
        "./shaders/density-average-vel.frag", GL_FRAGMENT_SHADER);
    GLuint copy_over_shader = get_shader("./shaders/copy-over.frag", GL_FRAGMENT_SHADER);
    GLuint step_down_shader = get_shader("./shaders/step-down.frag", GL_FRAGMENT_SHADER);
    GLuint step_horizontals_shader = get_shader(
        "./shaders/step-horizontals.frag", GL_FRAGMENT_SHADER);
    GLuint step_ups_shader = get_shader("./shaders/step-ups.frag", GL_FRAGMENT_SHADER);
    GLuint stream_down_shader = get_shader("./shaders/stream-down.frag", GL_FRAGMENT_SHADER);
    GLuint stream_horizontals_shader = get_shader(
        "./shaders/stream-horizontals.frag", GL_FRAGMENT_SHADER);
    GLuint stream_ups_shader = get_shader(
        "./shaders/stream-ups.frag", GL_FRAGMENT_SHADER);
    GLuint init_cond_shader = get_shader("./shaders/init-cond.frag", GL_FRAGMENT_SHADER);
    GLuint draw_shader = get_shader("./shaders/draw.frag", GL_FRAGMENT_SHADER);
    GLuint view_shader = get_shader("./shaders/view.frag", GL_FRAGMENT_SHADER);
    GLuint add_vel_shader = get_shader("./shaders/add-vel.frag", GL_FRAGMENT_SHADER);
    #endif

    GLuint density_program = make_program(
        vertices_shader, density_average_vel_shader);
    GLuint copy_program = make_program(vertices_shader, copy_over_shader);
    GLuint step_down_program = make_program(vertices_shader, step_down_shader);
    GLuint step_horizontals_program = make_program(
        vertices_shader, step_horizontals_shader);
    GLuint step_uppers_program = make_program(vertices_shader, step_ups_shader);
    GLuint stream_down_program = make_program(vertices_shader, stream_down_shader);
    GLuint stream_h_program = make_program(
        vertices_shader, stream_horizontals_shader);
    GLuint stream_ups_program = make_program(vertices_shader, stream_ups_shader);
    GLuint init_cond_program = make_program(vertices_shader, init_cond_shader);
    GLuint draw_program = make_program(vertices_shader, draw_shader);
    GLuint view_program = make_program(vertices_shader, view_shader);
    GLuint add_vel_program = make_program(vertices_shader, add_vel_shader);

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
        step_down_shader,
        step_horizontals_shader,
        step_ups_shader,
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
        step_down_program, 
        step_horizontals_program, 
        step_uppers_program, 
        stream_down_program, 
        stream_h_program, 
        stream_ups_program, 
        init_cond_program, 
        draw_program, 
        view_program, 
    };
    Quad view_frame = Quad::make_frame(width, height);
    Quad draw_frame = Quad::make_float_frame(width, height);
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

    /*for (auto &q: quads) {
        q->bind(init_cond_program);
        q->set_float_uniforms({
            {"left", 0.0}, {"centre", 0.0}, {"right", 2.0}
        });
    }*/

    auto set_init_cond = [&] {
        quads[1]->bind(init_cond_program);
        quads[1]->set_float_uniforms(
            {{"left", 0.0}, {"centre", 1.5}, {"right", 0.4}}
            );
        quads[1]->draw();
        unbind();
        quads[4]->bind(init_cond_program);
        quads[4]->set_float_uniforms(
            {{"left", 0.0}, {"centre", 1.5}, {"right", 0.4}}
            );
        quads[4]->draw();
        unbind();
    };

    auto draw = [&] {
        double w_stencil = 0.015;
        double h_stencil = 0.015;
        for (int i = 0; i < 6; i++) {
            draw_frame.bind(draw_program);
            draw_frame.set_int_uniform("tex", quads[i]->get_value());
            draw_frame.set_float_uniforms({
                {"xc", left_click.x}, {"yc", left_click.y}, 
                {"w", w_stencil}, {"h", h_stencil*((float)width/height)},
                {"r", 0.0}, {"g", 0.0}, {"b", 0.0}
            });
            draw_frame.draw();
            unbind();
            quads[i]->bind(copy_program);
            quads[i]->set_int_uniform("tex", draw_frame.get_value());
            quads[i]->draw();
            unbind();
        }
        draw_frame.bind(draw_program);
        draw_frame.set_int_uniform("tex", barrier.get_value());
        draw_frame.set_float_uniforms({
            {"xc", left_click.x}, {"yc", left_click.y}, 
            {"w", w_stencil}, {"h", h_stencil*((float)width/height)},
            {"r", 1.0}, {"g", 0.0}, {"b", 0.0}
        });
        draw_frame.draw();
        unbind();
        barrier.bind(copy_program);
        barrier.set_int_uniform("tex", draw_frame.get_value());
        barrier.draw();
        unbind();
    };

    int down1=0, horiz1=1, ups1=2;
    int down2=3, horiz2=4, ups2=5;
    double omega = 0.5;

    auto density_mean_vel = [&]{
        draw_frame.bind(density_program);
        draw_frame.set_int_uniforms({
            {"upTex", quads[ups1]->get_value()}, 
            {"leftCenterRightTex", quads[horiz1]->get_value()},
            {"downTex", quads[down1]->get_value()}});
        draw_frame.draw();
        unbind();
    };
    
    auto steps = [&]{

        quads[down2]->bind(step_down_program);
        quads[down2]->set_int_uniforms({
            {"downTex", quads[down1]->get_value()},
            {"densityVelTex", draw_frame.get_value()}
        });
        quads[down2]->set_float_uniforms({
            {"omega", omega},
        });
        quads[down2]->draw();
        unbind();

        quads[horiz2]->bind(step_horizontals_program);
        quads[horiz2]->set_int_uniforms({
            {"leftCenterRightTex", quads[horiz1]->get_value()},
            {"densityVelTex", draw_frame.get_value()}
        });
        quads[horiz2]->set_float_uniforms({
            {"omega", omega},
        });
        quads[horiz2]->draw();
        unbind();

        quads[ups2]->bind(step_uppers_program);
        quads[ups2]->set_int_uniforms({
            {"upTex", quads[ups1]->get_value()},
            {"densityVelTex", draw_frame.get_value()}
        });
        quads[ups2]->set_float_uniforms({
            {"omega", omega},
        });
        quads[ups2]->draw();
        unbind();

        swap(down1, down2);
        swap(horiz1, horiz2);
        swap(ups1, ups2);
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
            {"dy", 1.0/height}
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

        swap(down1, down2);
        swap(horiz1, horiz2);
        swap(ups1, ups2);
    };

    auto add_vel = [&] {
        quads[horiz2]->bind(add_vel_program);
        quads[horiz2]->set_int_uniform("tex", quads[horiz1]->get_value());
        quads[horiz2]->set_float_uniforms({
            {"left", 0.0}, {"centre", 1.0}, {"right", 0.25}, {"dx", 1.0/width}
        });
        quads[horiz2]->draw();
        unbind();
        swap(horiz1, horiz2);

    };

    #ifndef __EMSCRIPTEN__
    auto
    #endif
    loop = [&] {
        if (left_click.pressed) {
            draw();
        }
        if (left_click.released) {
            left_click.released = 0;
        }
        for (int p = 0; p < 5; p++) {
            density_mean_vel();
            steps();
        }
        streams();
        // add_vel();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        view_frame.bind(view_program);
        view_frame.set_int_uniform("tex", quads[horiz1]->get_value());
        view_frame.set_int_uniform("viewMode", 0);
        view_frame.draw();
        unbind();
        glfwPollEvents();
        left_click.update(window);
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
