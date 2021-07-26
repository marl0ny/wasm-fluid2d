#version 150 core

precision highp float;
varying vec2 st;
uniform sampler2D leftCenterRightTex;
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
    vec4 horizontals = texture2D(leftCenterRightTex, st);
    vec4 densityAvgVel = texture2D(densityVelTex, st);
    float density= densityAvgVel[0];
    vec2 avgVel = vec2(densityAvgVel[1], densityAvgVel[2]);

    // Centre Left (-1.0, 0.0)
    vec2 left = vec2(-1.0, 0.0);
    float leftTotal = getUpdatedTotal(left, avgVel, horizontals[0], density, 1.0/9.0);
    
    // Centre (0.0, 0.0)
    vec2 c = vec2(0.0, 0.0);
    float centreTotal = getUpdatedTotal(c, avgVel, horizontals[1], density, 4.0/9.0);

    // Centre Right (1.0, 0.0)
    vec2 right = vec2(1.0, 0.0);
    float rightTotal = getUpdatedTotal(right, avgVel, horizontals[2], density, 1.0/9.0);

    gl_FragColor = vec4(leftTotal, centreTotal, rightTotal, horizontals[3]);
    /*if (horizontals[3] == 0.0) {
        gl_FragColor = horizontals;
    } else {
        gl_FragColor = vec4(leftTotal, centreTotal, rightTotal, horizontals[3]);
    }*/

}