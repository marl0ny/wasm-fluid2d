#version 150 core

precision highp float;
varying vec2 st;
uniform sampler2D upTex;
uniform sampler2D leftCenterRightTex;
uniform sampler2D downTex;


void main() {
    vec4 up = texture2D(upTex, st);
    vec4 leftCenterRight = texture2D(leftCenterRightTex, st);
    vec4 down = texture2D(downTex, st);
    // Total number of particles
    float total = dot(up, vec4(1.0, 1.0, 1.0, 0.0));
    total      += dot(leftCenterRight, vec4(1.0, 1.0, 1.0, 0.0));
    total      += dot(down, vec4(1.0, 1.0, 1.0, 0.0));
    // Average velocity
    float vx = 0.0;
    float vy = 0.0;
    // all upper directions
    vx += -1.0*up[0]             + 1.0*up[2];
    vy +=  1.0*up[0] + 1.0*up[1] + 1.0*up[2];
    // left, right, and centre
    vx += -1.0*leftCenterRight[0] + 1.0*leftCenterRight[2];
    // all down directions
    vx += -1.0*down[0]               + 1.0*down[2];
    vy += -1.0*down[0] - 1.0*down[1] - 1.0*down[2];
    if (total != 0.0) {
        vx = vx/total;
        vy = vy/total;
    }
    // if (vx*vx + vy*vy > 100000.0) {
    //     vx = vx/(vx*vx + vy*vy);
    //     vy = vy/(vx*vx + vy*vy);
    // }
    gl_FragColor = vec4(total, vx, vy, 1.0);

}