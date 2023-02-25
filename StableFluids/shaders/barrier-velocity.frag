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
uniform sampler2D tex;


void main() {
    vec4 u = texture2D(tex, UV + vec2(0.0, dy/height));
    vec4 d = texture2D(tex, UV - vec2(0.0, dy/height));
    vec4 L = texture2D(tex, UV - vec2(dx/width, 0.0));
    vec4 r = texture2D(tex, UV + vec2(dx/width, 0.0));
    vec4 c = texture2D(tex, UV);
    vec3 res  = -(1.0 - c.a)*(u.rgb*u.a + d.rgb*d.a
                              + L.rgb*L.a + r.rgb*r.a);
    fragColor = vec4(c.rgb + res, c.a);
}