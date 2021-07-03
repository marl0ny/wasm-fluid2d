#version 150 core

precision highp float;
varying vec2 st;
uniform float left;
uniform float centre;
uniform float right;
uniform float xc;
uniform float yc;
uniform float dx;
uniform sampler2D tex;

void main() {
    if (st.x < dx) {
    vec4 color = texture2D(tex, st);
    color[3] = min(color[3], color[3] + right);
    gl_FragColor = color;
    } else {
    gl_FragColor = texture2D(tex, st);
    }
}