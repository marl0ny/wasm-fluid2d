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
    float ac = b[3];
    float au = texture2D(bTex, UV + vec2(0.0, dv))[3];
    float ad = texture2D(bTex, UV + vec2(0.0, -dv))[3];
    float al = texture2D(bTex, UV + vec2(-du, 0.0))[3];
    float ar = texture2D(bTex, UV + vec2(du, 0.0))[3];
    vec4 u0 = texture2D(lastIterTex, UV + vec2(0.0, dv));
    vec4 d0 = texture2D(lastIterTex, UV + vec2(0.0, -dv));
    vec4 l0 = texture2D(lastIterTex, UV + vec2(-du, 0.0));
    vec4 r0 = texture2D(lastIterTex, UV + vec2(du, 0.0));
    vec4 c0 = texture2D(lastIterTex, UV);
    vec4 u = u0*au + (1.0 - au)*c0;
    vec4 d = d0*ad + (1.0 - ad)*c0;
    vec4 l = l0*al + (1.0 - al)*c0;
    vec4 r = r0*ar + (1.0 - ar)*c0;
    // laplace = (u + d - 2.0*c)/(dy*dy) + (l + r - 2.0*c)/(dx*dx)
    // offdiag*v = (u + d)/(dy*dy) + (l + r)/(dx*dx)
    // diag = -2.0/(dx*dx) - 2.0/(dy*dy)
    vec4 num = b - (u + d)/(dy*dy) - (l + r)/(dx*dx);
    float den = -2.0/(dx*dx) - 2.0/(dy*dy);
    fragColor = ac*num/den
                 + (1.0 - ac)*u0*au + (1.0 - ac)*d0*ad
                 + (1.0 - ac)*l0*al + (1.0 - ac)*r0*ar;
}
