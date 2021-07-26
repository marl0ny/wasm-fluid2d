#version 330 core

precision highp float;
varying highp vec2 uv;
uniform float amplitude;
uniform float width;
uniform float height;
uniform float uc;
uniform float vc;
uniform float sx;
uniform float sy;
uniform float r;
uniform float g;
uniform float b;


void main() {
    float u = uv.x - uc;
    float v = uv.y - vc;
    float val = amplitude*exp(-u*u/(2.0*sx*sx))*exp(-v*v/(2.0*sy*sy));
    // gl_FragColor = vec4(r*val, g*val, b*val, 1.0);
    if (uv.x > 1.0/width && uv.x < 1.0 - 1.0/width && 
        uv.y > 1.0/height && uv.y < 1.0 - 1.0/height) {
        gl_FragColor = vec4(r*val, g*val, b*val, 1.0);
    } else {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}