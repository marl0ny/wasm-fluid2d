#version 150 core

precision highp float;
varying vec2 st;
uniform float dx;
uniform float left;
uniform float centre;
uniform float right;
uniform sampler2D barrierTex;

void main() {
    float barrier = texture2D(barrierTex, st)[0]; 
    vec3 col = (1.0 - barrier)*vec3(left, centre, right);
    gl_FragColor = vec4(col, 1.0);
    // || st.x >= 1.0 - dx
    if (st.x <= dx || st.x >= 1.0 - dx) {
        gl_FragColor[3] = 0.0;
    }
}
