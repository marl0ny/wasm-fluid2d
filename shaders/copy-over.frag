# version 150 core

precision highp float;
varying highp vec2 st;
uniform sampler2D tex;
uniform int viewMode;

void main() {
    highp vec4 col = texture2D(tex, st);
    gl_FragColor = col;
}