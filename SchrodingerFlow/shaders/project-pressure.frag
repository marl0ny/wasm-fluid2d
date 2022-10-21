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

uniform float hbar;
uniform sampler2D wavefuncTex;
uniform sampler2D pressureTex;

#define complex2 vec4

complex2 multiply(complex2 w, complex2 z) {
    return complex2(w[0]*z[0] - w[1]*z[1], w[0]*z[1] + w[1]*z[0],
                    w[2]*z[2] - w[3]*z[3], w[2]*z[3] + w[3]*z[2]);
}


void main() {
    complex2 psi = texture2D(wavefuncTex, UV);
    float pressure = texture2D(pressureTex, UV)[0];
    float angle = -pressure/hbar;
    complex2 e = complex2(cos(angle), sin(angle),
                          cos(angle), sin(angle));
    fragColor = multiply(psi, e);
}