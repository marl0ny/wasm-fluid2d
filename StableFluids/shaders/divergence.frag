#version 330 core

precision highp float;
varying highp vec2 uv;
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform int diffType;
uniform sampler2D vectTex;

const int CENTER = 0;
const int FOWARD = 1;
const int BACKWARD = 2;

void main() {
    float divVect;
    float du = dx/width;
    float dv = dy/height;
    if (diffType == CENTER) {
        vec4 right = texture2D(vectTex, vec2(uv.x + du, uv.y));
        vec4 left = texture2D(vectTex, vec2(uv.x - du, uv.y));
        float dvxdx = 0.5*(right - left).x/dx;
        vec4 up = texture2D(vectTex, vec2(uv.x, uv.y + dv));
        vec4 down = texture2D(vectTex, vec2(uv.x, uv.y - dv));
        float dvydy = 0.5*(up - down).y/dy;
        divVect = dvxdx + dvydy;
    } else if (diffType == FOWARD) {
        vec4 right = texture2D(vectTex, vec2(uv.x + du, uv.y));
        vec4 center = texture2D(vectTex, vec2(uv.x, uv.y));
        float dvxdx = (right - center).x/dx;
        vec4 up = texture2D(vectTex, vec2(uv.x, uv.y + dv));
        float dvydy = (up - center).y/dy;
        divVect = dvxdx + dvydy;
    } else if (diffType == BACKWARD) {
        vec4 left = texture2D(vectTex, vec2(uv.x - du, uv.y));
        vec4 center = texture2D(vectTex, vec2(uv.x, uv.y));
        float dvxdx = (center - left).x/dx;
        vec4 down = texture2D(vectTex, vec2(uv.x, uv.y - dv));
        float dvydy = (center - down).y/dy;
        divVect = dvxdx + dvydy;
    }
    gl_FragColor = vec4(divVect, divVect, divVect, 1.0);
}