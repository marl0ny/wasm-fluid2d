#version 330 core

precision highp float;

#if __VERSION__ >= 300
in vec2 uv;
out vec4 fragColor;
#define texture2D texture
#else
#define fragColor gl_FragColor
varying highp vec2 uv;
#endif

uniform sampler2D texCol;
uniform sampler2D texVel;

void main() {
    vec4 col = texture2D(texCol, uv);
    float rho = texture2D(texVel, uv)[2];
    fragColor = vec4(rho*col[0], rho*col[1], rho*col[2], 1.0);
}