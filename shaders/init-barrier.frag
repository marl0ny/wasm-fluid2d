#version 150 core

precision highp float;
varying vec2 st;
uniform float dy;

void main() {
    float x = 0.75*(st.x - 0.5);
    float y = st.y - 0.5;
    /*if ((x*x + y*y) < 0.05*0.05) {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        if (st.y <= dy || st.y >= 1.0 - dy) 
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        else
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }*/
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
