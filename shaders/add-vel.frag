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
    if (st.x < dx)
    gl_FragColor = vec4(left, centre, right, 1.0);
    else
    gl_FragColor = texture2D(tex, st);
}