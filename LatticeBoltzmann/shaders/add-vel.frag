#version 150 core

precision highp float;
varying vec2 st;
uniform float left;
uniform float centre;
uniform float right;
uniform float sx;
uniform float sy;
uniform float xc;
uniform float yc;
uniform sampler2D tex;

void main() {
    float x = st.x - xc;
    float y = st.y - yc;
    float dist = exp(-0.5*(x*x/(sx*sx) + y*y/(sy*sy)));
    vec3 newParticles = dist*vec3(left, centre, right);
    gl_FragColor = texture2D(tex, st) + vec4(newParticles, 0.0);
}