#version 150 core

precision highp float;
varying highp vec2 st;
uniform sampler2D downTex;
uniform sampler2D horizontalsTex;
uniform sampler2D upsTex;
uniform sampler2D barrierTex;
uniform sampler2D densityVelTex;
uniform float dx;
uniform float dy;
uniform int viewMode;
uniform float contrast;
uniform float brightness;


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
        float vx1 = texture2D(densityVelTex, vec2(st.x, st.y - dy))[1];
        float vx2 = texture2D(densityVelTex, vec2(st.x, st.y + dy))[1];
        float vy1 = texture2D(densityVelTex, vec2(st.x - dx, st.y))[2];
        float vy2 = texture2D(densityVelTex, vec2(st.x + dx, st.y))[2];
        float curl = vx2 - vx1 - (vy2 - vy1);
        col = contrast*(1.0 - b[0])*vec4(brightness*0.25 + abs(15.0*curl)*
                                colour2D(curl, curl), 1.0);  
    }
    else if (viewMode == 1) {
        // vec4 tmp = vec4(1.0, 1.0, 1.0, 0.0);
        // float a = dot(d, tmp) + dot(h, tmp) + dot(u, tmp);
        float a = texture2D(densityVelTex, st)[0];
        a = contrast*0.8*a + brightness - 2.0;
        col = vec4(vec3(a, a, a), 1.0);
    } else if (viewMode == 2) {
        vec2 v = texture2D(densityVelTex, st).gb;
        col = contrast*(1.0 - b[0])*vec4(brightness*0.25 + 5.0*sqrt(v.x*v.x + v.y*v.y)*
                                colour2D(v.y, v.x), 1.0); 
    }
    gl_FragColor = col;
}
