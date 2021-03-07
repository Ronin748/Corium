//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_ENTITY_H
#define CORIUM_ENTITY_H

#include "game/obj/obj.h"

class Entity : public Obj {
public:
	float health = 100.0f;

	bool colliding = false;

	bool onGround = true;

	float ticks = 0.0f;

	bool sprinting = false;

	bool hurt = false;

	bool jumping = false;

	bool moving = false;

	float spd = 1.0f;

	// find this in world tick (probably player)
	Entity *closestEnemy = NULL;

	float chaseRange = 8.0f;
	float attackRange = 1.0f;

	// ai thinks
	void think();

	// entity movement, otherwise idle
	void chase(Entity *another);

	// he attacc
	void attack(Entity *another);

	// bounce up and down or whatever
	void idle();

	void draw(Renderer *renderer);

	void drawShadows(Renderer *renderer);

	void tick();

	Entity() : Obj()
	{

	}
};

#endif //CORIUM_ENTITY_H
