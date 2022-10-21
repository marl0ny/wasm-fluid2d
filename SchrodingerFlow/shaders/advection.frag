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
uniform sampler2D velocityTex;
uniform float width;
uniform float height;
uniform sampler2D densityTex;


void main() {
    vec2 velocity = texture2D(velocityTex, UV).xy;
    vec2 velUV = vec2(velocity.x/width, velocity.y/height);
    fragColor = texture2D(densityTex, UV - velUV.xy*dt);
}