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
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform sampler2D forceTex;
uniform sampler2D uTex;

uniform int boundaryMode;

const int PERIODIC =  0;
const int NO_SLIP = 1;
const int FREE_SLIP = 2;


void main() {
    vec4 F = texture2D(forceTex, UV);
    vec4 u = texture2D(uTex, UV);
    fragColor = vec4(0.0, 0.0, 0.0, 0.0);
    if (boundaryMode == PERIODIC) {
        fragColor = vec4(dt*F.rgb + u.rgb, 1.0);
    } else if (boundaryMode == NO_SLIP || boundaryMode == FREE_SLIP) {
        if (UV.x > dx/width && UV.x < 1.0 - dx/width && 
            UV.y > dy/height && UV.y < 1.0 - dy/height)
            fragColor = vec4(dt*F.rgb + u.rgb, 1.0);
        if (((UV.x - 0.5)*(UV.x - 0.5) + (UV.y - 0.5)*(UV.y - 0.5)) > 0.5*0.5)
            fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        /*if (((UV.x - 0.5)*(UV.x - 0.5) + (UV.y - 0.5)*(UV.y - 0.5)) < 0.05*0.05)
            fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (((UV.x - 0.35)*(UV.x - 0.35)
             + (UV.y - 0.35)*(UV.y - 0.35)) < 0.05*0.05)
            fragColor = vec4(0.0, 0.0, 0.0, 0.0);*/
    }
}
