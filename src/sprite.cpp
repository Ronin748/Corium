//
// Created by Roninkoi on 20.12.2015.
//

#include "sprite.h"

void Sprite::tickAnim(float newTicks)
{
	ticks = newTicks;

	if (animTicks - ticks >= animSpd) {
		++anim;
		if (anim >= anim_cycle.size()) {
			anim = 0;
		}
	}

	currentSprite = &sprites[anim];
}

void Sprite::setAnim(std::vector<int> newAnim, float newSpd)
{
	anim_cycle = newAnim;
	animSpd = newSpd;
}

void Sprite::addSprite(glm::vec4 newSprite)
{
	sprites[sprites.size() - 1] = newSprite;
	sprites.resize(sprites.size() + 1);

	currentSprite = &sprites[anim];
}
