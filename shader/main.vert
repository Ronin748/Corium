#version 330 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 norm;
layout (location = 2) in vec4 tex;
layout (location = 3) in vec4 col;

out vec3 fPos;
out vec3 fNorm;
out vec2 fTex;
out vec4 fCol;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(pos.xyz, 1.0f);
    fPos = vec3(model * pos);
    fNorm = transpose(inverse(mat3(model))) * (norm.xyz);
    fTex = vec2((tex.s)*tex.p, (tex.t)*tex.q);
    fCol = col;
}
