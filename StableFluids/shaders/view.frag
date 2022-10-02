#version 330 core

precision highp float;
varying highp vec2 uv;
uniform sampler2D tex;
uniform float dx;
uniform float dy;

void main() {
    // float width = 512.0; float height = 512.0;
    // float dx = 1.0/width;
    // float dy = 1.0/height;
    float vx1 = texture2D(tex, vec2(uv.x, uv.y - dy)).x;
    float vx2 = texture2D(tex, vec2(uv.x, uv.y + dy)).x;
    float vy1 = texture2D(tex, vec2(uv.x - dx, uv.y)).y;
    float vy2 = texture2D(tex, vec2(uv.x + dx, uv.y)).y;
    float curl = 200.0*(vx2 - vx1 - (vy2 - vy1));
    vec4 col = texture2D(tex, uv);
    float val = 4.0*sqrt(col[0]*col[0] + col[1]*col[1]);
    gl_FragColor = vec4(curl, 0.3*abs(curl), -curl, 1.0);
}