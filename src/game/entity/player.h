//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_PLAYER_H
#define CORIUM_PLAYER_H

#include "game/obj/camera.h"
#include "anim.h"
#include "entity.h"
#include "input.h"
#include "phys/sys.h"

class Player : public Entity {
public:
	Camera *camera;

	Anims playerAnims;

	glm::vec3 renderPos = glm::vec3(0.0f);

	glm::vec3 playerMov = glm::vec3(0.0f);
	glm::vec3 playerLastMov = glm::vec3(0.0f, 0.0f, -1.0f);
	float lastMovAngle = 0.0f;

	Mesh clipMesh;

	bool flaming = false;
	bool flying = false;

	glm::vec3 startPos = glm::vec3(0.0f);
	glm::vec3 startRot = glm::vec3(0.0f);

	void getPlayerCam();

	void loadPlayer();

	void restartPlayer();

	void draw(Renderer *renderer);

	void drawShadows(Renderer *renderer);

	void collide(Sys *ps);

	void getInput(Input *input);

	void tick();

	Player() : Entity()
	{
	}
};

#endif //CORIUM_PLAYER_H
