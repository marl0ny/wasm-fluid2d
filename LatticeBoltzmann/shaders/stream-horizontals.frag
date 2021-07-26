#version 150 core

precision highp float;
varying vec2 st;
uniform sampler2D centreTex;
uniform sampler2D barrierTex;
uniform float dx;
uniform float dy;
uniform float srcLeft;
uniform float srcCentre;
uniform float srcRight;

vec4 getIncoming(vec2 from) {
    float barrier = texture2D(barrierTex, st + from)[0]; 
    return (1.0 - barrier)*texture2D(centreTex, st + from);
}

vec4 getReflecting(vec2 from) {
    float barrier = texture2D(barrierTex, st + from)[0]; 
    return barrier*texture2D(centreTex, st);
}

void main() {

    float left = getIncoming(vec2(dx, 0.0))[0];
    float centre = getIncoming(vec2(0.0, 0.0))[1];
    float right = getIncoming(vec2(-dx, 0.0))[2];
    left += getReflecting(vec2(dx, 0.0))[2];
    right += getReflecting(vec2(-dx, 0.0))[0];

    float notSrc = texture2D(centreTex, st)[3];

    gl_FragColor = vec4(left*notSrc + (1.0 - notSrc)*srcLeft, 
                        centre*notSrc + (1.0 - notSrc)*srcCentre, 
                        right*notSrc + (1.0 - notSrc)*srcRight, notSrc);
}