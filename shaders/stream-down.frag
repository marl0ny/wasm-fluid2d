#version 150 core

precision highp float;
varying vec2 st;
uniform sampler2D upsTex;
uniform sampler2D downTex;
uniform sampler2D barrierTex;
uniform float dx;
uniform float dy;

vec4 getIncoming(vec2 from) {
    float barrier = texture2D(barrierTex, st + from)[0]; 
    return (1.0 - barrier)*texture2D(downTex, st + from);
}

vec4 getReflecting(vec2 from) {
    float barrier = texture2D(barrierTex, st + from)[0]; 
    return barrier*texture2D(upsTex, st);
}

void main() {

    // get upper left cell's down left stream
    float downLeft = getIncoming(vec2(dx, dy))[0];
    // get upper cell's downward stream
    float downDown = getIncoming(vec2(0.0, dy))[1];
    // get upper left cell's down right stream
    float downRight = getIncoming(vec2(-dx, dy))[2];
    downLeft += getReflecting(vec2(dx, dy))[2]; // reflect upper right
    downDown += getReflecting(vec2(0.0, dy))[1]; // reflect upper centre
    downRight += getReflecting(vec2(-dx, dy))[0]; // reflect upper left

    gl_FragColor = vec4(downLeft, downDown, downRight, 1.0);
}