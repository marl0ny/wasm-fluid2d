#version 150 core

precision highp float;
varying vec2 st;
uniform float dx;
uniform float dy;
uniform int barrierType;

void main() {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (barrierType == 0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else if (barrierType == 1) {
        if (st.y <= dy || st.y >= 1.0 - dy) 
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        else
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else if (barrierType == 2) {
        float x = st.x - 0.5;
        float y = st.y - 0.5;
        if ((x*x + y*y) < 0.05*0.05) {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    } else if (barrierType == 3) {
        float x = st.x - 0.5;
        float y = st.y - 0.5;
        if (y > -0.15 && y < 0.15 && x > -0.001 && x < 0.001) {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    } else if (barrierType == 4) {
        float x = (dy/dx)*(st.x - 0.5);
        float y = st.y - 0.5;
        if ((x*x + y*y) < 0.15*0.15) {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    }
    /* if (st.x <= dx && st.x >= 1.0 - dx) {
        gl_FragColor += vec4(0.0, 1.0, 0.0, 0.0);
    } */
}
