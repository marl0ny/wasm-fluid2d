#version 330 core

precision highp float;

#if __VERSION__ >= 300
in vec2 UV;
out vec4 fragColor;
#define texture2D texture
#else
#define fragColor fragColor
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
    vec4 p = texture2D(pressureTex, UV);
    vec4 dpdx = (texture2D(pressureTex, vec2(UV.x + du, UV.y))
                 - texture2D(pressureTex, vec2(UV.x - du, UV.y))
                )/(2.0*dx);
    vec4 dpdy = (texture2D(pressureTex, vec2(UV.x, UV.y + dv))
                 - texture2D(pressureTex, vec2(UV.x, UV.y - dv))
                )/(2.0*dy);
    vec4 u = texture2D(uTex, UV);
    fragColor = vec4(u.x - dpdx.x, u.y - dpdy.y, u.z, u.a);
}
