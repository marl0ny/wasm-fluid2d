#version 330 core

precision highp float;
varying highp vec2 uv;
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
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
    // laplace = (u + d - 2.0*c)/(dy*dy) + (l + r - 2.0*c)/(dx*dx)
    // offdiag*v = (u + d)/(dy*dy) + (l + r)/(dx*dx)
    // diag = -2.0/(dx*dx) - 2.0/(dy*dy)
    vec4 num = b - (u + d)/(dy*dy) - (l + r)/(dx*dx);
    float den = -2.0/(dx*dx) - 2.0/(dy*dy);
    gl_FragColor = num/den;
}
