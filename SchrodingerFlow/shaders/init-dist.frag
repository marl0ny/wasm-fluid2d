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

#define complex vec2
#define complex2 vec4

uniform float amplitude;
uniform float sigma_x;
uniform float sigma_y;
uniform float u0;
uniform float v0;
uniform float nx;
uniform float ny;
uniform complex2 spin;


const float TAU = 6.283185307179586;

complex multiply(complex w, complex z) {
    return complex(w.x*z.x - w.y*z.y, w.x*z.y + w.y*z.x);
}

complex conjugate(complex z) {
    return complex(z.x, -z.y);
}

float complex2Length(complex2 z) {
    return sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2] + z[3]*z[3]);
}

void main() {
    float u = UV[0] - u0;
    float v = UV[1] - v0;
    float sx = sigma_x;
    float sy = sigma_y;
    complex2 nSpin = spin/complex2Length(spin);
    complex2 nSpinT = complex2(conjugate(-nSpin.zw),
                               conjugate(nSpin.xy));
    float dist = exp(-u*u/(2.0*sx*sx) - v*v/(2.0*sy*sy));
    // float dist = 1.0;
    float angle = TAU*(nx*UV[0] + ny*UV[1]);
    angle = (dist > 0.2)? angle: 0.0;
    // if (dist <=  0.5 && dist > 0.1) angle = angle*smoothstep(0.1, 0.5, dist);
    complex e = complex(cos(angle), sin(angle));
    complex2 c1 = amplitude*complex2(multiply(e, nSpin.xy),
                                          multiply(e, nSpin.zw));
    float a2 = 0.01*amplitude; // *sqrt(1.0 - dist*dist);
    complex e2 = complex(1.0, 0.0);
    complex2 c2 = a2*complex2(multiply(e2, nSpinT.xy),
                              multiply(e2, nSpinT.zw));
    fragColor = (c1 + c2)/complex2Length(c1 + c2);
}