#VERSION_NUMBER_PLACEHOLDER

precision highp float;

#if __VERSION__ >= 300
in vec2 UV;
out vec4 fragColor;
#define texture2D texture
#else
#define fragColor gl_FragColor
varying highp vec2 UV;
#endif

uniform sampler2D tex;
uniform float amplitude;
uniform float uc;
uniform float vc;
uniform float sx;
uniform float sy;
uniform float r;
uniform float g;
uniform float b;


void main() {
    float u = UV.x - uc;
    float v = UV.y - vc;
    vec4 val = texture2D(tex, UV);
    float dist = amplitude*exp(-u*u/(2.0*sx*sx))*exp(-v*v/(2.0*sy*sy));
    fragColor = vec4(val.r + r*dist, val.g + g*dist, val.b + b*dist, 1.0);
}