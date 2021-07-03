#version 330 core

precision highp float;
varying highp vec2 uv;
uniform float dt;
uniform sampler2D velocityTex;
uniform sampler2D positionTex;


void main() {
    vec4 u = texture2D(velocityTex, uv);
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

    col += texture2D(positionTex, uv - u.xy*dt);

    /*float width = 256;
    float height = 256;
    ivec2 intLookupPos = ivec2(int(width*(uv.x - u.x*dt)), 
                               int(height*(uv.y - u.y*dt)));
    col += texelFetch(positionTex, intLookupPos, 0);*/

    gl_FragColor = col;
}