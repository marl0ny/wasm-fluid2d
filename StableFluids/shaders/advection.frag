#version 330 core

precision highp float;

#if __VERSION__ >= 300
in vec2 UV;
out vec4 fragColor;
#define texture2D texture
#else
#define fragColor fragColor
varying highp vec2 UV;
#endif

uniform float dt;
uniform sampler2D velocity1Tex;
uniform sampler2D velocity2Tex;


void main() {
    vec4 u = texture2D(velocity1Tex, UV);
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
    col += texture2D(velocity2Tex, UV - u.xy*dt);
    fragColor = vec4(col.rgb*u.a*col.a, u.a);
}