//
// Created by Roninkoi on 18.12.2015.
//
#include "util/objParser.h"
#include "obj.h"

void Obj::update()
{
	phys.m = pow(fabs(physMesh.bsRadius), 2) * 0.75f * M_PI + 1.0f;
	phys.I = phys.m * physMesh.bsRadius * physMesh.bsRadius * 0.4f;

	phys.update();

	if (hasMesh) updateMesh();
}

void Obj::updateMesh()
{
	// transform mesh according to phys
	mesh.objMatrix = phys.getRMatrix();
	if (!ro) {
		mesh.update();
	}
	physMesh.objMatrix = phys.getMatrix();
	physMesh.update();
}

void Obj::draw(Renderer *renderer)
{
	if (rendered && visible) {
		if (!ro)
			mesh.draw(renderer, &tex);
		else
			render(renderer);
	}
}

void Obj::render(Renderer *renderer)
{
	mesh.render(renderer, &tex);
}

void Obj::drawShadows(Renderer *renderer)
{
	if (!ro)
		mesh.drawShadows(renderer);
	else
		renderShadows(renderer);
}

void Obj::renderShadows(Renderer *renderer)
{
	mesh.renderShadows(renderer);
}

void Obj::loadMesh(std::string sp, std::string pp)
{
	if (pp == "")
		pp = sp;

	mesh = loadObj(sp);
	physMesh = loadObj(pp);

	hasMesh = true;
}

void Obj::loadObject(std::string tp, std::string sp, std::string pp)
{
	loadMesh(sp, pp);
	tex.loadTexture(tp);
}

void Obj::destroyObject()
{
	tex.deleteTexture();
}
