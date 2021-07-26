set IMGUI_SRC=./imgui/imgui.cpp ./imgui/imgui_draw.cpp ./imgui/imgui_tables.cpp ./imgui/imgui_widgets.cpp ./imgui/imgui_demo.cpp ./imgui/imgui_impl_opengl3.cpp ./imgui/imgui_impl_glfw.cpp
python make_shaders_h.py
emcc -o fluid.js gl_wrappers2d/gl_wrappers.cpp %IMGUI_SRC% fluid2d.cpp -I./imgui -std=c++11 -O0 -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1

