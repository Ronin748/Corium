#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 tex;
layout(location = 2) in vec4 col;

out vec2 vTex;
out vec4 vCol;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj*view*model*vec4(pos.xyz, 1.0f);
    vTex = vec2(tex.s*tex.p, tex.t*tex.q);
    vCol = col;
}
