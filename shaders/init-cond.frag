#version 150 core

precision highp float;
varying vec2 st;
uniform float left;
uniform float centre;
uniform float right;

void main() {
    gl_FragColor = vec4(left, centre, right, 1.0);
    /*
    float right2 = right;
    if (st.x < 0.1) right2 = 0.5*centre;
    gl_FragColor = vec4(left, centre, right2, 1.0);*/
}
