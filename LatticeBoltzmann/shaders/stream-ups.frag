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
    return (1.0 - barrier)*texture2D(upsTex, st + from);
}

vec4 getReflecting(vec2 from) {
    float barrier = texture2D(barrierTex, st + from)[0]; 
    return barrier*texture2D(downTex, st);
}

void main() {

    float upperLeft = getIncoming(vec2(dx, -dy))[0];
    float up = getIncoming(vec2(0.0, -dy))[1];
    float upperRight = getIncoming(vec2(-dx, -dy))[2];
    upperLeft += getReflecting(vec2(dx, -dy))[2];
    up += getReflecting(vec2(0.0, -dy))[1];
    upperRight += getReflecting(vec2(-dx, -dy))[0];
    float notSrc = texture2D(upsTex, st)[3];

    gl_FragColor = vec4(notSrc*upperLeft, 
                        notSrc*up, 
                        notSrc*upperRight, notSrc);
}