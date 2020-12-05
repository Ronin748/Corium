//
// Created by Roninkoi on 20.12.2015.
//

#ifndef CORIUM_TEXTURE_H
#define CORIUM_TEXTURE_H

#include "corium.h"

class Texture {
public:
	std::string path = "";

	int w = 0, h = 0;

	GLuint tex = 0;

	GLuint loadTexture(std::string path);

	void deleteTexture()
	{
		if (tex > 0) {
			glDeleteTextures(1, &tex);
			path = "";
			tex = 0;
		}
	}

	Texture()
	{
		path = "";
	}
/*
   ~Texture()
   {
        deleteTexture();
   }*/
};

unsigned char *loadGLTex(std::string p, int *w, int *h);

#endif //CORIUM_TEXTURE_H
