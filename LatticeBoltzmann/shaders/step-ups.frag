#version 150 core

precision highp float;
varying vec2 st;
uniform sampler2D upTex;
uniform sampler2D densityVelTex;
uniform float omega;

float getApproxBoltzmannFactor(vec2 dir, vec2 avgVel) {
    return (1.0 + 3.0*dot(dir, avgVel)
             + 4.5*dot(dir, avgVel)*dot(dir, avgVel)
             - (3.0/2.0)*dot(avgVel, avgVel));
}

float getTempTotal(vec2 dir, vec2 avgVel, 
                        float density, float weight) {
    float e = getApproxBoltzmannFactor(dir, avgVel);
    return density*weight*e;
}

float getUpdatedTotal(vec2 dir, vec2 avgVel,
                        float oldTotal, float density, float weight) {
    float total = getTempTotal(dir, avgVel, density, weight);
    return oldTotal + omega*(total - oldTotal);
}

void main() {
    vec4 up = texture2D(upTex, st);
    vec4 densityAvgVel = texture2D(densityVelTex, st);
    float density= densityAvgVel[0];
    vec2 avgVel = densityAvgVel.yz;

    // Upper Left (-1.0, 1.0)
    vec2 upLeft = vec2(-1.0, 1.0);
    float upLeftTotal = getUpdatedTotal(upLeft, avgVel, up[0], density, 1.0/36.0);
    
    // Up (0.0, 1.0)
    vec2 upUp = vec2(0.0, 1.0);
    float upTotal = getUpdatedTotal(upUp, avgVel, up[1], density, 1.0/9.0);

    // Upper Right (1.0, 1.0)
    vec2 upRight = vec2(1.0, 1.0);
    float upRightTotal = getUpdatedTotal(upRight, avgVel, up[2], density, 1.0/36.0);

    gl_FragColor = vec4(upLeftTotal, upTotal, upRightTotal, 1.0);

}