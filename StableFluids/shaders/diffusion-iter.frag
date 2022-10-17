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
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform float nu;
uniform sampler2D bTex;
uniform sampler2D lastIterTex;


void main() {
    float du = dx/width;
    float dv = dy/height;
    vec4 b = texture2D(bTex, uv);
    vec4 u = texture2D(lastIterTex, uv + vec2(0.0, dv));
    vec4 d = texture2D(lastIterTex, uv + vec2(0.0, -dv));
    vec4 l = texture2D(lastIterTex, uv + vec2(-du, 0.0));
    vec4 r = texture2D(lastIterTex, uv + vec2(du, 0.0));
    vec4 num = b + dt*nu*((u + d)/(dy*dy) + (l + r)/(dx*dx));
    float den = (1.0 + 2.0*dt*nu/(dx*dx) + 2.0*dt*nu/(dy*dy));
    vec4 col = num/den;
    fragColor = col;
}
