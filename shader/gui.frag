#version 330 core

layout(location = 0) out vec4 fragCol;

in vec2 vTex;
in vec4 vCol;

uniform sampler2D tex;

void main() {
    fragCol = texture(tex, vTex) * vCol;
}
