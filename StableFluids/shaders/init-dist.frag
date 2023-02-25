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

uniform float amplitude;
uniform float width;
uniform float height;
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
    float val = amplitude*exp(-u*u/(2.0*sx*sx))*exp(-v*v/(2.0*sy*sy));
    // fragColor = vec4(r*val, g*val, b*val, 1.0);
    if (UV.x > 1.0/width && UV.x < 1.0 - 1.0/width && 
        UV.y > 1.0/height && UV.y < 1.0 - 1.0/height) {
        fragColor = vec4(r*val, g*val, b*val, 1.0);
    } else {
        fragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}