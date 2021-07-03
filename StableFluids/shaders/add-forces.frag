#version 330 core

precision highp float;
varying highp vec2 uv;
uniform float dt;
uniform sampler2D forceTex;
uniform sampler2D uTex;


void main() {
    vec4 F = texture2D(forceTex, uv);
    vec4 u = texture2D(uTex, uv);
    vec4 u2 = u + F*dt;
    gl_FragColor = vec4(u2.rgb, 1.0);
}
