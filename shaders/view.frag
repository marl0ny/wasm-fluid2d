#version 150 core

precision highp float;
varying highp vec2 st;
uniform sampler2D downTex;
uniform sampler2D horizontalsTex;
uniform sampler2D upsTex;
uniform sampler2D barrierTex;
uniform int viewMode;


vec3 colour2D(float x, float y) {
    float pi = 3.141592653589793;
    float argVal = atan(x, y);
    float maxCol = 1.0;
    float minCol = 50.0/255.0;
    float colRange = maxCol - minCol;
    if (argVal <= pi/3.0 && argVal >= 0.0) {
        return vec3(maxCol,
                    minCol + colRange*argVal/(pi/3.0), minCol);
    } else if (argVal > pi/3.0 && argVal <= 2.0*pi/3.0){
        return vec3(maxCol - colRange*(argVal - pi/3.0)/(pi/3.0),
                    maxCol, minCol);
    } else if (argVal > 2.0*pi/3.0 && argVal <= pi){
        return vec3(minCol, maxCol,
                    minCol + colRange*(argVal - 2.0*pi/3.0)/(pi/3.0));
    } else if (argVal < 0.0 && argVal > -pi/3.0){
        return vec3(maxCol, minCol,
                    minCol - colRange*argVal/(pi/3.0));
    } else if (argVal <= -pi/3.0 && argVal > -2.0*pi/3.0){
        return vec3(maxCol + (colRange*(argVal + pi/3.0)/(pi/3.0)),
                    minCol, maxCol);
    } else if (argVal <= -2.0*pi/3.0 && argVal >= -pi){
        return vec3(minCol,
                    minCol - (colRange*(argVal + 2.0*pi/3.0)/(pi/3.0)), maxCol);
    }
    else {
        return vec3(minCol, maxCol, maxCol);
    }
}


void main() {
    vec4 b = texture2D(barrierTex, st);
    vec4 d = texture2D(downTex, st);
    vec4 h = texture2D(horizontalsTex, st);
    vec4 u = texture2D(upsTex, st);
    vec4 col;
    if (viewMode == 0) {
        float curl = (u[2] - u[0]) - (d[2] - d[0]);
        float a = 20.0*abs(curl);
        // col = (1.0 - b[0])*vec4(0.5 + 20.0*vec3(abs(curl)), 1.0);
        col = (1.0 - b[0])*vec4(h[1]/4.0 
                                 + a*colour2D(curl, curl), 1.0);
    }
    else if (viewMode == 1) {
        float a = 2.0*(h[1] - 0.5);
        col = (1.0 - b[0])*vec4(a*colour2D(h[2], h[0]), 1.0);
    } else if (viewMode == 2) {
        float a = 2.0*h[1] - 1.0;
        col = vec4(vec3(a, a, a), 1.0);
    } else if (viewMode == 3) {
        float vDiff = u[2] - u[0];
        float hDiff = d[2] - d[0];
        float a = 0.75 + 10.0*(vDiff*vDiff + hDiff*hDiff);
        col = (1.0 - b[0])*vec4(a*colour2D(hDiff, vDiff), 1.0);
    }
    gl_FragColor = col;
}
