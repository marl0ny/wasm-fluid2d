#version 330 core

precision highp float;
varying highp vec2 uv;
uniform sampler2D tex;

void main() {
    vec4 col = texture2D(tex, uv);
    gl_FragColor = vec4(col[2], col[2], col[2], 1.0);
}