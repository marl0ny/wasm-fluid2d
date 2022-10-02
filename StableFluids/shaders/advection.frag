#version 330 core

precision highp float;
varying highp vec2 uv;
uniform float dt;
uniform sampler2D velocity1Tex;
uniform sampler2D velocity2Tex;


void main() {
    vec4 u = texture2D(velocity1Tex, uv);
    vec4 col = texture2D(velocity2Tex, uv - u.xy*dt);
    gl_FragColor = vec4(col[0]*u[2], col[1]*u[3], u[2], u[3]);
}