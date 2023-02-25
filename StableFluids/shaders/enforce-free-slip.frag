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

uniform sampler2D uTex;


void main() {
    float du = dx/width;
    float dv = dy/height;
    vec4 u = texture2D(uTex, UV);
    float aC = u.a;
    float aU = texture2D(uTex, UV + vec2(0.0, dv)).a;
    float aD = texture2D(uTex, UV - vec2(0.0, dv)).a;
    float aR = texture2D(uTex, UV + vec2(du, 0.0)).a;
    float aL = texture2D(uTex, UV - vec2(du, 0.0)).a;
    fragColor = aC*vec4(aL*aR*u.x, aU*aD*u.y, u.z, aC);
}