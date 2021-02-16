#version 150 core

precision highp float;
varying vec2 st;
uniform float xc;
uniform float yc;
uniform float w;
uniform float h;
uniform float r;
uniform float g;
uniform float b;
uniform sampler2D tex;


void main() {
    float hw = w/2.0;
    float hh = h/2.0;
    vec4 col = texture2D(tex, st);
    if (xc >= (st.x - hw) && xc <= (st.x + hw)) {
        if (yc >= (st.y - hh) && yc <= (st.y + hh)) {
            col.r = r;
            col.g = g;
            col.b = b;
        }  
    }
    gl_FragColor = col;
}