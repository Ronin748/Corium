//
// Created by Roninkoi on 18.9.2017.
//

#ifndef CORIUM_CAMERA_H
#define CORIUM_CAMERA_H

#include "obj.h"

class Camera {
public:
	glm::vec3 pos = glm::vec3(0.0f);

	glm::vec3 rot = glm::vec3(0.0f);

	float z = 7.0f;

	float fov = 1.8f;

	bool camAI = false;

	glm::vec3 camStart = glm::vec3(0.0f);

	Camera()
	{

	}
};

#endif //CORIUM_CAMERA_H
