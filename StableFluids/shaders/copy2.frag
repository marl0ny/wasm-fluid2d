#version 330 core

precision highp float;
varying highp vec2 uv;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    vec4 v1 = texture2D(tex1, uv);
    vec4 v2 = texture2D(tex2, uv);
    vec4 v3 = v1 + v2;
    gl_FragColor = vec4(v3.xyz, 1.0);
}