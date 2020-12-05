//
// Created by Roninkoi on 22.12.2015.
//

#ifndef CORIUM_MESH_H
#define CORIUM_MESH_H

#include "render/renderer.h"
#include "phys/phys.h"

const float MAX_TRIANGLE_SIZE = 200.0f;

class Mesh {
public:
	float triangle_size = 1.0f;

	std::vector<float> vertexData0;
	std::vector<float> vertexData;
	std::vector<float> texData;
	std::vector<float> colData;
	std::vector<float> normalData;
	std::vector<float> vertexNormalData;

	std::vector<int> indexData;

	glm::vec3 collisionNormal;
	glm::vec3 collisionCenter;

	float bsRadius = 1.0f;
	glm::vec3 bsCenter = glm::vec3(0.0f);

	glm::vec3 velocity = glm::vec3(0.0f);
	int staticity = 1;

	bool isIntersecting = false;

	glm::mat4 objMatrix = glm::mat4(1.0f);

	bool intersects(Mesh *another);

	bool sphereIsect(Mesh *another, glm::vec3 s, float r);

	void getNormals();

// bounding sphere intersection test
	bool bsIsect(Mesh *another);

	void getBoundingSphere();

	void update();

	std::vector<float> transform(std::vector<float> data, glm::mat4 matrix);

	void triangleSizeCheck();

	void setCol(glm::vec4 newCol);

	void draw(Renderer *renderer, Texture *tex);

	void render(Renderer *renderer, Texture *tex);

	void drawShadows(Renderer *renderer);

	void renderShadows(Renderer *renderer);

	Mesh()
	{
	}
};

#endif //CORIUM_MESH_H
