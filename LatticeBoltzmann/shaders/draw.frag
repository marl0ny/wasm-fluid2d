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
uniform int drawType;
uniform sampler2D tex;

#define SQUARE 0
#define CIRCLE 1


void main() {
    float hw = w/2.0;
    float hh = h/2.0;
    vec4 col = texture2D(tex, st);
    float x = st.x - xc;
    float y = st.y - yc;
    if (drawType == CIRCLE && 
        ((h*h)*x*x/(w*w) + y*y) <= (hw*hw + hh*hh)) {
        col.r = r;
        col.g = g;
        col.b = b;
    }else if (drawType == SQUARE) {
        if (xc >= (st.x - hw) && xc <= (st.x + hw)) {
            if (yc >= (st.y - hh) && yc <= (st.y + hh)) {
                col.r = r;
                col.g = g;
                col.b = b;
            }  
        }
    }
    gl_FragColor = col;
}
