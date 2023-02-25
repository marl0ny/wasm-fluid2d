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
uniform float dx;
uniform float dy;
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
    vec4 val = texture2D(tex, UV);
    float val = amplitude*exp(-u*u/(2.0*sx*sx))*exp(-v*v/(2.0*sy*sy));
    // gl_FragColor = vec4(r*val, g*val, b*val, 1.0);
    if (UV.x > dx/width && UV.x < 1.0 - dx/width && 
        UV.y > dy/height && UV.y < 1.0 - dy/height) {
        fragColor = vec4(val.r + r*val, val.g + g*val, val.b + b*val, 1.0);
    } else {
        fragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}