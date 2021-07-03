#version 330 core

precision highp float;
varying highp vec2 uv;
uniform sampler2D tex;

void main() {
    vec4 col = texture2D(tex, uv);
    gl_FragColor = vec4(col.b, col.b, col.b, 1.0);
}