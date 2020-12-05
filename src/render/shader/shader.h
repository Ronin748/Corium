//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_SHADER_H
#define CORIUM_SHADER_H

#include "corium.h"

class Shader {
public:
	GLuint program;

	GLuint vertexShaderID;
	GLuint geometryShaderID;
	GLuint fragmentShaderID;

	std::string vertPath;
	std::string gsPath;
	std::string fragPath;

	int max_lights = 8;

	GLuint loadShaders(std::string vp, std::string fp);

	GLuint loadGS(std::string vp, std::string gp, std::string fp);

	std::string readShaderSource(std::string path);

	void init(std::string vp, std::string fp)
	{
		program = loadShaders(vp, fp);
	}

	void initGS(std::string vp, std::string gp, std::string fp)
	{
		program = loadGS(vp, gp, fp);
	}

	void destroyShader() const
	{
		glDeleteProgram(program);
	}

	Shader()
	{
	}
};

#endif //CORIUM_SHADER_H
