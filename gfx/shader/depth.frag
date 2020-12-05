#version 330
//#extension GL_EXT_geometry_shader4 : enable
//#extension GL_ARB_explicit_attrib_location : enable

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDistance = length(fragPos.xyz - lightPos);

    lightDistance = lightDistance / farPlane;

    gl_FragDepth = lightDistance;
}
