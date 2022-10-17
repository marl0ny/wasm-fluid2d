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

uniform float dt;
uniform sampler2D velocity1Tex;
uniform sampler2D velocity2Tex;


void main() {
    vec4 u = texture2D(velocity1Tex, uv);
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
    col += texture2D(velocity2Tex, uv - u.xy*dt);
    fragColor = vec4(col.rgb*u.a*col.a, u.a);
}