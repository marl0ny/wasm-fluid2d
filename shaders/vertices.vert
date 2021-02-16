# version 150 core

attribute vec3 position;
varying highp vec2 st;

void main () {
    gl_Position = vec4(position.xyz, 1.0);
    st = vec2(0.5, 0.5) + position.xy/2.0;
}
