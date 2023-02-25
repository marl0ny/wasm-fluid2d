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
uniform sampler2D pressureTex;
uniform sampler2D uTex;


void main() {
    float du = dx/width;
    float dv = dy/height;
    vec4 u = texture2D(uTex, UV);
    // Find whether there is a barrier at the sampled texel location.
    // A value of 0.0 means a barrier is assigned to the texel.
    float aC = u.a;
    float aU = texture2D(uTex, UV + vec2(0.0, dv)).a;
    float aD = texture2D(uTex, UV - vec2(0.0, dv)).a;
    float aR = texture2D(uTex, UV + vec2(du, 0.0)).a;
    float aL = texture2D(uTex, UV - vec2(du, 0.0)).a;
    vec4 pC = texture2D(pressureTex, UV);
    vec4 pU0 = texture2D(pressureTex, vec2(UV.x, UV.y + dv));
    vec4 pD0 = texture2D(pressureTex, vec2(UV.x, UV.y - dv));
    vec4 pR0 = texture2D(pressureTex, vec2(UV.x + du, UV.y));
    vec4 pL0 = texture2D(pressureTex, vec2(UV.x - du, UV.y));
    vec4 pU = aU*pU0 + pC*(1.0 - aU);
    vec4 pD = aD*pD0 + pC*(1.0 - aD);
    vec4 pR = aR*pR0 + pC*(1.0 - aR);
    vec4 pL = aL*pL0 + pC*(1.0 - aL);
    vec4 dpdx = (pR - pL)/(2.0*dx);
    vec4 dpdy = (pU - pD)/(2.0*dy);
    fragColor = vec4(u.x - dpdx.x, u.y - dpdy.y, u.z, aC);
}
