//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_OBJ_H
#define CORIUM_OBJ_H

#include "texture.h"
#include "phys/phys.h"
#include "mesh.h"

class Obj {
public:
	Mesh mesh;
	Mesh physMesh;
	Phys phys;

	Texture tex;

	bool rendered = true;
	bool shadows = true;
	bool visible = true;
	bool ro = false;     // render only

	void draw(Renderer *renderer);

	void render(Renderer *renderer);

	void drawShadows(Renderer *renderer);

	void renderShadows(Renderer *renderer);

	void update();

	void updateMesh();

	bool hasMesh = false;

	void loadMesh(std::string sp, std::string pp = "");

	void loadObject(std::string sp, std::string tp, std::string pp = "");

	void destroyObject();

	Obj()
	{
	}
};

#endif //CORIUM_OBJ_H
