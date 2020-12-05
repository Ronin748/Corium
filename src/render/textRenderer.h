//
// Created by Roninkoi on 29.7.2017.
//

#ifndef CORIUM_TEXTRENDERER_H
#define CORIUM_TEXTRENDERER_H

#include "corium.h"
#include "render/shader/shader.h"
#include "quad.h"

class TextRenderer {
public:
	Shader *textShader;

	GLuint renderToTex(std::string s)
	{
		return *(new GLuint);
	}

	void draw(std::string s, glm::mat4 objMatrix, GLuint font_tex)
	{
		Quad character;
	}

	TextRenderer()
	{
	}
};

#endif //CORIUM_TEXTRENDERER_H
