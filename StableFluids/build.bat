python make_shaders_h.py
emcc -o fluid.js gl_wrappers2d/gl_wrappers.cpp fluid2d.cpp -std=c++11 -O0 -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1
