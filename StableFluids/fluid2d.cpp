#include "gl_wrappers2d/gl_wrappers.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <utility>
#ifdef __EMSCRIPTEN__
#include <functional>
#include <emscripten.h>
#include "shaders.hpp"
#endif


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
        this->dx = x - x_prev;
        this->dy = y - y_prev;
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

    int width = 256, height = 256;
    const double DT = 0.1;
    const double Lx = 1.0, Ly = 1.0;
    float view_ratio = 2.0;

    GLFWwindow *window = init_window(view_ratio*width, 
                                     view_ratio*height);
    init_glew();
    glViewport(0, 0, width, height);
    MouseClick left_click = MouseClick(GLFW_MOUSE_BUTTON_1);
    // MouseClick right_click = MouseClick(GLFW_MOUSE_BUTTON_2);

    #ifndef __EMSCRIPTEN__
    GLuint vertices_shader = get_shader("./shaders/vertices.vert", 
                                        GL_VERTEX_SHADER);
    GLuint view_shader = get_shader("./shaders/view.frag", 
                                    GL_FRAGMENT_SHADER);
    GLuint add_forces_shader = get_shader("./shaders/add-forces.frag", 
                                          GL_FRAGMENT_SHADER);
    GLuint advection_shader = get_shader("./shaders/advection.frag",
                                         GL_FRAGMENT_SHADER);
    GLuint copy2_shader = get_shader("./shaders/copy2.frag",
                                     GL_FRAGMENT_SHADER);
    GLuint diffusion_iter_shader = get_shader("./shaders/diffusion-iter.frag",
                                              GL_FRAGMENT_SHADER);
    GLuint init_dist_shader = get_shader("./shaders/init-dist.frag",
                                          GL_FRAGMENT_SHADER);
    GLuint pressure_iter_shader = get_shader("./shaders/pressure-iter.frag",
                                             GL_FRAGMENT_SHADER);
    GLuint subtract_gradp_shader = get_shader("./shaders/subtract-gradp.frag",
                                              GL_FRAGMENT_SHADER);
    GLuint divergence_shader = get_shader("./shaders/divergence.frag",
                                          GL_FRAGMENT_SHADER);
    #else
    GLuint vertices_shader = make_vertex_shader(vertices_shader_source);
    GLuint view_shader = make_fragment_shader(view_shader_source);
    GLuint add_forces_shader = make_fragment_shader(add_forces_shader_source);
    GLuint advection_shader = make_fragment_shader(advection_shader_source);
    GLuint copy2_shader = make_fragment_shader(copy2_shader_source);
    GLuint diffusion_iter_shader = 
        make_fragment_shader(diffusion_iter_shader_source);
    GLuint init_dist_shader = make_fragment_shader(init_dist_shader_source);
    GLuint pressure_iter_shader = 
        make_fragment_shader(pressure_iter_shader_source);
    GLuint subtract_gradp_shader = 
        make_fragment_shader(subtract_gradp_shader_source);
    GLuint divergence_shader = make_fragment_shader(divergence_shader_source);

    #endif

    GLuint view_program = make_program(vertices_shader, view_shader);
    GLuint add_forces_program = make_program(vertices_shader, 
                                             add_forces_shader);
    GLuint advection_program = make_program(vertices_shader, 
                                            advection_shader);
    GLuint copy2_program = make_program(vertices_shader, copy2_shader);
    GLuint diffusion_iter_program = make_program(vertices_shader, 
                                                 diffusion_iter_shader);
    GLuint init_fluid_program = make_program(vertices_shader,
                                             init_dist_shader);
    GLuint pressure_iter_program = make_program(vertices_shader,
                                                pressure_iter_shader);
    GLuint divergence_program = make_program(vertices_shader,
                                             divergence_shader);
    GLuint subtract_gradp_program = make_program(vertices_shader, 
                                                  subtract_gradp_shader);

    Quad view_frame = Quad::make_frame(view_ratio*width, view_ratio*height);
    // Quad d0 = Quad::make_float_frame(width, height);
    Quad q0 = Quad::make_float_frame(width, height);
    Quad q1 = Quad::make_float_frame(width, height);
    Quad q2 = Quad::make_float_frame(width, height);
    Quad p1 = Quad::make_float_frame(width, height);
    Quad p2 = Quad::make_float_frame(width, height);
    Quad forces = Quad::make_float_frame(width, height);
    Quad densities = Quad::make_float_frame(width, height);
    Quad blank = Quad::make_float_frame(width, height);

    auto copy_tex = [&](Quad *dest, Quad *src) {
        dest->bind(copy2_program);
        dest->set_int_uniform("tex1", src->get_value());
        dest->set_int_uniform("tex2", Quad::get_blank());
        dest->draw();
        unbind();
    };

    auto init_fluid = [&]()  {
        q1.bind(init_fluid_program);
        q1.set_float_uniforms({{"amplitude", 2.0}, 
                               {"uc", 0.25}, {"vc", 0.25}, 
                               {"sx", 0.1}, {"sy", 0.1},
                               {"r", 1.0}, {"g", 1.0}, {"b", 1.0},
                               {"width", width}, {"height", height}});
        q1.draw();
        q2.bind(init_fluid_program);
        q2.set_float_uniforms({{"amplitude", 2.0}, 
                               {"uc", 0.75}, {"vc", 0.75}, 
                               {"sx", 0.1}, {"sy", 0.1}, 
                               {"r", -1.0}, {"g", -1.0}, {"b", 1.0},
                               {"width", width}, {"height", height}});
        q2.draw();
        unbind();
        q0.bind(copy2_program);
        q0.set_int_uniforms({{"tex1", q1.get_value()}, 
                             {"tex2", q2.get_value()}});
        q0.draw();
        unbind();
    };

    auto advect = [&](Quad *dest, Quad *src) {
        dest->bind(advection_program);
        dest->set_int_uniform("velocity1Tex", src->get_value());
        dest->set_int_uniform("velocity2Tex", src->get_value());
        dest->set_float_uniform("dt", DT);
        dest->draw();
        unbind();
    };

    auto diffuse = [&](Quad *iter_quads[2], Quad *init_quad, double v,
                       int n_iterations) -> Quad * {
        if (v == 0) return iter_quads[0];
        int next = 1, prev = 0;
        for (int i = 0; i < n_iterations; i++) {
            iter_quads[next]->bind(diffusion_iter_program);
            iter_quads[next]->set_int_uniforms(
                {{"btex", init_quad->get_value()}, 
                 {"lastIterTex", iter_quads[prev]->get_value()}
                }
            );
            iter_quads[next]->set_float_uniforms(
                {{"dx", Lx/width}, {"dy", Ly/height},
                 {"width", Lx}, {"height", Ly},
                 {"dt", DT},
                 {"nu", v}
                }
            );
            iter_quads[next]->draw();
            unbind();
            std::swap(next, prev);
        }
        return iter_quads[prev];
    };

    auto draw_forces = [&](Quad *f, double x, double y, 
                           double dx, double dy) {
        f->bind(init_fluid_program);
        f->set_float_uniforms({
            {"amplitude", 1.0}, 
            {"uc", x}, {"vc", y}, 
            {"sx", 0.05}, {"sy", 0.05},
            {"r", 10.0*dx}, {"g", 10.0*dy}, {"b", 1.0},
            {"width", width}, {"height", height}
        });
        f->draw();
        unbind();
    };

    auto add_forces = [&](Quad *uf, Quad *f, Quad *ui) {
        uf->bind(add_forces_program);
        uf->set_float_uniform("dt", DT);
        uf->set_int_uniforms({
            {"forceTex", f->get_value()}, 
            {"uTex", ui->get_value()}
        });
        uf->draw();
        unbind();
    };

    auto divergence = [&](Quad *dest, Quad *src) {
        dest->bind(divergence_program);
        dest->set_float_uniforms({
            {"dx", Lx/(double)width}, {"dy", Ly/(double)height},
            {"width", Lx}, {"height", Ly}
        });
        dest->set_int_uniforms({
            {"diffType", 0}, {"vectTex", src->get_value()}
        });
        dest->draw();
        unbind();
    };

    auto pressure = [&](Quad *iter_quads[2], Quad *init_quad, 
                        int n_iterations) -> Quad * {
        int next = 1, prev = 0;
        for (int i = 0; i < n_iterations; i++) {
            iter_quads[next]->bind(pressure_iter_program);
            iter_quads[next]->set_float_uniforms({
                {"dx", 1.0/(float)width}, {"dy", 1.0/(float)height},
                {"width", Lx}, {"height", Ly}
            });
            iter_quads[next]->set_int_uniforms({
                {"bTex", init_quad->get_value()}, 
                {"lastIterTex", iter_quads[prev]->get_value()},
            });
            iter_quads[next]->draw();
            unbind();
            std::swap(next, prev);
        }
        return iter_quads[prev];
    };

    auto subtract_gradp = [&](Quad *u, Quad *w, Quad *p) {
        u->bind(subtract_gradp_program);
        u->set_float_uniforms({
            {"dx", Lx/(float)width}, {"dy", Ly/(float)height},
            {"width", Lx}, {"height", Ly}
        });
        u->set_int_uniforms({
            {"pressureTex", p->get_value()}, 
            {"uTex", w->get_value()}
        });
        u->draw();
        unbind();
    };

    init_fluid();
    copy_tex(&q1, &q0);
    Quad *quads[2] = {&q1, &q2};

    #ifndef __EMSCRIPTEN__
    auto
    #endif
    loop = [&] {
        if (left_click.pressed) {
            draw_forces(&forces, left_click.x, left_click.y,
                        left_click.dx, left_click.dy);
        }
        glViewport(0, 0, view_ratio*width, view_ratio*height); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        view_frame.bind(view_program);
        view_frame.set_int_uniform("tex", quads[0]->get_value());
	view_frame.set_float_uniforms({{"dx", Lx/(float)width},
				       {"dy", Ly/(float)height}});
        view_frame.draw();
        unbind();
        glViewport(0, 0, width, height);
    
        advect(quads[1], quads[0]);
        std::swap(quads[0], quads[1]);
        copy_tex(&q0, quads[0]);
        Quad *q = diffuse(quads, &q0, 0.0, 10);
        if (q != quads[0]) std::swap(quads[0], quads[1]);
        if (left_click.pressed)
            add_forces(quads[1], &forces, quads[0]);
        else
            // add_forces(quads[1], &blank, quads[0]);
            std::swap(quads[1], quads[0]);
        // std::swap(quads[1], quads[0]);
        divergence(&q0, quads[1]);
        // copy_tex(&p1, quads[1]);
        Quad *p_arr[2] = {&p1, &p2};
        Quad *p = pressure(p_arr, &q0, 10);
        subtract_gradp(quads[0], quads[1], p);

        glfwPollEvents();
        left_click.update(window);
        glfwSwapBuffers(window);
    };
    #ifndef __EMSCRIPTEN__
    while(!glfwWindowShouldClose(window)) {
        loop();
    }
    #else
    emscripten_set_main_loop(browser_loop, 0, true);
    #endif
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
