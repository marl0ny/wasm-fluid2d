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

uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform sampler2D bTex;
uniform sampler2D lastIterTex;


void main() {
    float du = dx/width;
    float dv = dy/height;
    vec4 b = texture2D(bTex, UV);
    vec4 u = texture2D(lastIterTex, UV + vec2(0.0, dv));
    vec4 d = texture2D(lastIterTex, UV + vec2(0.0, -dv));
    vec4 l = texture2D(lastIterTex, UV + vec2(-du, 0.0));
    vec4 r = texture2D(lastIterTex, UV + vec2(du, 0.0));
    // laplace = (u + d - 2.0*c)/(dy*dy) + (l + r - 2.0*c)/(dx*dx)
    // offdiag*v = (u + d)/(dy*dy) + (l + r)/(dx*dx)
    // diag = -2.0/(dx*dx) - 2.0/(dy*dy)
    vec4 num = b - (u + d)/(dy*dy) - (l + r)/(dx*dx);
    float den = -2.0/(dx*dx) - 2.0/(dy*dy);
    fragColor = num/den;
}
