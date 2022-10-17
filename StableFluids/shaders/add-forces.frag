#version 330 core

precision highp float;

#if __VERSION__ >= 300
in vec2 uv;
out vec4 fragColor;
#define texture2D texture
#else
#define fragColor gl_FragColor
varying highp vec2 uv;
#endif

uniform float dt;
uniform sampler2D forceTex;
uniform sampler2D uTex;


void main() {
    vec4 F = texture2D(forceTex, uv);
    vec4 u = texture2D(uTex, uv);
    if (uv.x > 1.0/256.0 && uv.x < 1.0 - 1.0/256.0 && 
        uv.y > 1.0/256.0 && uv.y < 1.0 - 1.0/256.0) {
        fragColor = vec4(dt*F.rgb + u.rgb, 1.0);
    } else {
        fragColor = vec4(dt*F.rgb + u.rgb, 0.0);
    }
}
