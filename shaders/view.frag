# version 150 core

precision highp float;
varying highp vec2 st;
uniform sampler2D tex;
uniform int viewMode;

void main() {
    highp vec4 col = texture2D(tex, st);
    if (viewMode == 0) {
    }
    if (viewMode == 1) {
        float d = col[1];
        col[0] = d;
        col[2] = d;
    }
    gl_FragColor = col;
}
