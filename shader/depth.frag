#version 330 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDist = length(fragPos.xyz - lightPos);

    lightDist = lightDist / farPlane;

    gl_FragDepth = lightDist;
}
