#version 150 core

precision highp float;
varying vec2 st;
uniform sampler2D downTex;
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
    vec4 down = texture2D(downTex, st);
    vec4 densityAvgVel = texture2D(densityVelTex, st);
    float density= densityAvgVel[0];
    vec2 avgVel = vec2(densityAvgVel[1], densityAvgVel[2]);

    // Down Left (-1.0, -1.0)
    vec2 downLeft = vec2(-1.0, -1.0);
    float downLeftTotal = getUpdatedTotal(downLeft, avgVel, down[0], density, 1.0/36.0);
    
    // Down (0.0, -1.0)
    vec2 downDown = vec2(0.0, -1.0);
    float downTotal = getUpdatedTotal(downDown, avgVel, down[1], density, 1.0/9.0);

    // Down Right (1.0, -1.0)
    vec2 downRight = vec2(1.0, -1.0);
    float downRightTotal = getUpdatedTotal(downRight, avgVel, down[2], density, 1.0/36.0);

    gl_FragColor = vec4(downLeftTotal, downTotal, downRightTotal, down[3]);
    /*if (down[3] == 0.0) {
        gl_FragColor = down;
    } else {
        gl_FragColor = vec4(downLeftTotal, downTotal, downRightTotal, down[3]);
    }*/

}
