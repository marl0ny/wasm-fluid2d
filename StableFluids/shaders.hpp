#ifndef _SHADERS_H_
#define _SHADERS_H_
const char *copy2_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    vec4 v1 = texture2D(tex1, uv);
    vec4 v2 = texture2D(tex2, uv);
    vec4 v3 = v1 + v2;
    gl_FragColor = vec4(v3.rg, v1.b, v1.a);
})";
const char *vertices_shader_source = R"(

precision highp float;
attribute vec3 position;
varying highp vec2 uv;

void main() {
    gl_Position = vec4(position.xyz, 1.0);
    uv = vec2(0.5, 0.5) + position.xy/2.0;
}
)";
const char *add_forces_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform float dt;
uniform sampler2D forceTex;
uniform sampler2D uTex;


void main() {
    vec4 F = texture2D(forceTex, uv);
    vec4 u = texture2D(uTex, uv);
    gl_FragColor = vec4((u + F*dt).rg, u.b, u.a);
}
)";
const char *diffusion_iter_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform float dt;
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform float nu;
uniform sampler2D bTex;
uniform sampler2D lastIterTex;


void main() {
    float du = dx/width;
    float dv = dy/height;
    vec4 b = texture2D(bTex, uv);
    vec4 u = texture2D(lastIterTex, uv + vec2(0.0, dv));
    vec4 d = texture2D(lastIterTex, uv + vec2(0.0, -dv));
    vec4 l = texture2D(lastIterTex, uv + vec2(-du, 0.0));
    vec4 r = texture2D(lastIterTex, uv + vec2(du, 0.0));
    vec4 num = b + dt*nu*((u + d)/(dy*dy) + (l + r)/(dx*dx));
    float den = (1.0 + 2.0*dt*nu/(dx*dx) + 2.0*dt*nu/(dy*dy));
    vec4 col = num/den;
    gl_FragColor = col;
}
)";
const char *view_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform sampler2D tex;
uniform float dx;
uniform float dy;

void main() {
    // float width = 512.0; float height = 512.0;
    // float dx = 1.0/width;
    // float dy = 1.0/height;
    float vx1 = texture2D(tex, vec2(uv.x, uv.y - dy)).x;
    float vx2 = texture2D(tex, vec2(uv.x, uv.y + dy)).x;
    float vy1 = texture2D(tex, vec2(uv.x - dx, uv.y)).y;
    float vy2 = texture2D(tex, vec2(uv.x + dx, uv.y)).y;
    float curl = 200.0*(vx2 - vx1 - (vy2 - vy1));
    vec4 col = texture2D(tex, uv);
    float val = 4.0*sqrt(col[0]*col[0] + col[1]*col[1]);
    gl_FragColor = vec4(curl, 0.3*abs(curl), -curl, 1.0);
})";
const char *pressure_iter_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform sampler2D bTex;
uniform sampler2D lastIterTex;


void main() {
    float du = dx/width;
    float dv = dy/height;
    vec4 b = texture2D(bTex, uv);
    vec4 u = texture2D(lastIterTex, uv + vec2(0.0, dv));
    vec4 d = texture2D(lastIterTex, uv + vec2(0.0, -dv));
    vec4 l = texture2D(lastIterTex, uv + vec2(-du, 0.0));
    vec4 r = texture2D(lastIterTex, uv + vec2(du, 0.0));
    // laplace = (u + d - 2.0*c)/(dy*dy) + (l + r - 2.0*c)/(dx*dx)
    // offdiag*v = (u + d)/(dy*dy) + (l + r)/(dx*dx)
    // diag = -2.0/(dx*dx) - 2.0/(dy*dy)
    vec4 num = b - (u + d)/(dy*dy) - (l + r)/(dx*dx);
    float den = -2.0/(dx*dx) - 2.0/(dy*dy);
    gl_FragColor = num/den;
}
)";
const char *divergence_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform int diffType;
uniform sampler2D vectTex;

const int CENTER = 0;
const int FOWARD = 1;
const int BACKWARD = 2;

void main() {
    float divVect;
    float du = dx/width;
    float dv = dy/height;
    if (diffType == CENTER) {
        vec4 right = texture2D(vectTex, vec2(uv.x + du, uv.y));
        vec4 left = texture2D(vectTex, vec2(uv.x - du, uv.y));
        float dvxdx = 0.5*(right - left).x/dx;
        vec4 up = texture2D(vectTex, vec2(uv.x, uv.y + dv));
        vec4 down = texture2D(vectTex, vec2(uv.x, uv.y - dv));
        float dvydy = 0.5*(up - down).y/dy;
        divVect = dvxdx + dvydy;
    } else if (diffType == FOWARD) {
        vec4 right = texture2D(vectTex, vec2(uv.x + du, uv.y));
        vec4 center = texture2D(vectTex, vec2(uv.x, uv.y));
        float dvxdx = (right - center).x/dx;
        vec4 up = texture2D(vectTex, vec2(uv.x, uv.y + dv));
        float dvydy = (up - center).y/dy;
        divVect = dvxdx + dvydy;
    } else if (diffType == BACKWARD) {
        vec4 left = texture2D(vectTex, vec2(uv.x - du, uv.y));
        vec4 center = texture2D(vectTex, vec2(uv.x, uv.y));
        float dvxdx = (center - left).x/dx;
        vec4 down = texture2D(vectTex, vec2(uv.x, uv.y - dv));
        float dvydy = (center - down).y/dy;
        divVect = dvxdx + dvydy;
    }
    gl_FragColor = vec4(divVect, divVect, divVect, divVect);
})";
const char *init_dist_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform float amplitude;
uniform float width;
uniform float height;
uniform float uc;
uniform float vc;
uniform float sx;
uniform float sy;
uniform float r;
uniform float g;
uniform float b;


void main() {
    float u = uv.x - uc;
    float v = uv.y - vc;
    float val = amplitude*exp(-u*u/(2.0*sx*sx))*exp(-v*v/(2.0*sy*sy));
    gl_FragColor = vec4(r*val, g*val, 1.0, 1.0);
    if (uv.x <= 1.0/width || uv.x >= 1.0 - 1.0/width) {
        gl_FragColor[2] = 0.0;
    }
    if (uv.y <= 1.0/height || uv.y >= 1.0 - 1.0/height) {
        gl_FragColor[3] = 0.0;
    }
})";
const char *advection_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform float dt;
uniform sampler2D velocity1Tex;
uniform sampler2D velocity2Tex;


void main() {
    vec4 u = texture2D(velocity1Tex, uv);
    vec4 col = texture2D(velocity2Tex, uv - u.xy*dt);
    gl_FragColor = vec4(col[0]*u[2], col[1]*u[3], u[2], u[3]);
})";
const char *subtract_gradp_shader_source = R"(

precision highp float;
varying highp vec2 uv;
uniform float dx;
uniform float dy;
uniform float width;
uniform float height;
uniform sampler2D pressureTex;
uniform sampler2D uTex;


void main() {
    float du = dx/width;
    float dv = dy/height;
    vec4 p = texture2D(pressureTex, uv);
    vec4 dpdx = (texture2D(pressureTex, vec2(uv.x + du, uv.y))
                 - texture2D(pressureTex, vec2(uv.x - du, uv.y))
                )/(2.0*dx);
    vec4 dpdy = (texture2D(pressureTex, vec2(uv.x, uv.y + dv))
                 - texture2D(pressureTex, vec2(uv.x, uv.y - dv))
                )/(2.0*dy);
    vec4 u = texture2D(uTex, uv);
    gl_FragColor = vec4(u.x - dpdx.x, u.y - dpdy.y, u.z, u.a);
}
)";
#endif