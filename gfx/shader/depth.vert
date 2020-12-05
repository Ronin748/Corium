#version 330
//#extension GL_EXT_geometry_shader4 : enable
//#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec4 position;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(position.xyz, 1.0f);
}
