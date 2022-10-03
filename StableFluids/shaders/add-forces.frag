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

uniform float dt;
uniform sampler2D forceTex;
uniform sampler2D uTex;


void main() {
    vec4 F = texture2D(forceTex, UV);
    vec4 u = texture2D(uTex, UV);
    fragColor = vec4(dt*F.rgb + u.rgb, 1.0);
}
