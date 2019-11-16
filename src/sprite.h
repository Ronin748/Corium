//
// Created by Roninkoi on 20.12.2015.
//

#ifndef CORIUM_SPRITE_H
#define CORIUM_SPRITE_H

#include "texture.h"

class Sprite {
public:
Texture tex;

std::vector <glm::vec4> sprites;
glm::vec4 *currentSprite;

std::vector <int> anim_cycle;
int anim;
float animSpd;
float animTicks;

float ticks;

void tickAnim(float newTicks);

void setAnim(std::vector <int> newAnim, float newSpd);
void addSprite(glm::vec4 newSprite);

void loadSprite(std::string path)
{
        currentSprite = &sprites[anim];
        tex.loadTexture(path.c_str());
}

Sprite()
{
        sprites.resize(1);
        sprites[0] = glm::vec4(1.0, 1.0, 1.0, 1.0);

        anim_cycle = {0};
        currentSprite = &sprites[anim];
}
};

#endif //CORIUM_SPRITE_H
