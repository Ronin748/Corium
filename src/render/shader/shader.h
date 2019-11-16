//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_SHADER_H
#define CORIUM_SHADER_H

#include "corium.h"
//#include "shadersrc.h" // this causes errors

class Shader {
public:
GLuint program;

GLuint VertexShaderID;
GLuint GeometryShaderID;
GLuint FragmentShaderID;

int max_lights = 8;

GLuint loadShaders(const char *vertPath, const char *fragPath, bool src);

GLuint loadGS(const char *vertPath, const char *fragPath, const char *gsPath, bool src);

void init(const char *vertPath, const char *fragPath, bool ls)
{
        program = loadShaders(vertPath, fragPath, !ls);
}

void initGS(const char *vertPath, const char *fragPath, const char *gsPath, bool ls)
{
        program = loadGS(vertPath, fragPath, gsPath, !ls);
}

void destroyShader()
{
        glDeleteProgram(program);
}

Shader()
{
}
};

#endif //CORIUM_SHADER_H
