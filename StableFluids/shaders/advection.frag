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

uniform float dt;
// uniform float dx;
// uniform float dy;
// uniform float width;
// uniform float height;
uniform sampler2D advectorTex;
uniform sampler2D advecteeTex;

// uniform int boundaryMode;

// #define PERIODIC 1
// #define NO_SLIP 0


void main() {
    vec4 v = texture2D(advectorTex, UV);
    vec4 u = texture2D(advecteeTex, UV - v.xy*dt);
    // (v1.a*v2.a)
    fragColor = vec4(u.rgb, v.a);
}