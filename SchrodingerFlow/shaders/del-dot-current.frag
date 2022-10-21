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
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform float normFactor;
uniform bool isDensityConstant;
uniform sampler2D wavefuncTex;

#define complex2 vec4


const complex2 IMAG_UNIT = complex2(0.0, 1.0, 0.0, 1.0);

complex2 multiply(complex2 w, complex2 z) {
    return complex2(w[0]*z[0] - w[1]*z[1], w[0]*z[1] + w[1]*z[0],
                    w[2]*z[2] - w[3]*z[3], w[2]*z[3] + w[3]*z[2]);
}

complex2 conj(complex2 z) {
    return complex2(z[0], -z[1], z[2], -z[3]);
}

complex2 laplacian2ndOrder4Point(sampler2D wavefuncTex) {
    complex2 u = texture2D(wavefuncTex, UV + vec2(0.0, dy/height));
    complex2 d = texture2D(wavefuncTex, UV - vec2(0.0, dy/height));
    complex2 l = texture2D(wavefuncTex, UV - vec2(dx/width, 0.0));
    complex2 r = texture2D(wavefuncTex, UV + vec2(dx/width, 0.0));
    complex2 c = texture2D(wavefuncTex, UV);
    return (u + d - 2.0*c)/(dy*dy) + (l + r - 2.0*c)/(dx*dx);
}

complex2 laplacian4thOrder9Point(sampler2D wavefuncTex) {
    complex2 u2 = texture2D(wavefuncTex, UV + 2.0*vec2(0.0, dy/height));
    complex2 u1 = texture2D(wavefuncTex, UV +     vec2(0.0, dy/height));
    complex2 d1 = texture2D(wavefuncTex, UV -     vec2(0.0, dy/height));
    complex2 d2 = texture2D(wavefuncTex, UV - 2.0*vec2(0.0, dy/height));
    complex2 L2 = texture2D(wavefuncTex, UV - 2.0*vec2(dx/width, 0.0));
    complex2 L1 = texture2D(wavefuncTex, UV -     vec2(dx/width, 0.0));
    complex2 r1 = texture2D(wavefuncTex, UV +     vec2(dx/width, 0.0));
    complex2 r2 = texture2D(wavefuncTex, UV + 2.0*vec2(dx/width, 0.0));
    complex2 c0 = texture2D(wavefuncTex, UV);
    float dy2 = dy*dy, dx2 = dx*dx;
    return (-u2/12.0 + 4.0*u1/3.0 - 5.0*c0/2.0 + 4.0*d1/3.0 - d2/12.0)/dy2
            + (-L2/12.0 + 4.0*L1/3.0 - 5.0*c0/2.0 + 4.0*r1/3.0 - r2/12.0)/dx2;
}

void main() {
    complex2 psi = texture2D(wavefuncTex, UV);
    complex2 absPsi2 = multiply(conj(psi), psi);
    float density = absPsi2[0] + absPsi2[2];
    complex2 laplacianPsi = laplacian4thOrder9Point(wavefuncTex);
    if (isDensityConstant) {
        complex2 tmp;
        tmp = multiply(conj(psi), multiply(IMAG_UNIT, laplacianPsi));
        fragColor = (-hbar/density)*vec4(1.0, 1.0,
                                         1.0, 1.0)*(tmp[0] + tmp[2]);
    } else {
        // TODO !
    }
}

