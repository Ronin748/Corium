//
// Created by Roninkoi on 8.4.2017.
//

#ifndef CORIUM_ANIM_H
#define CORIUM_ANIM_H

#include "corium.h"
#include "mesh.h"

class Anim {
public:
std::vector<Mesh> frames;
Mesh currentFrame;

Phys phys;
Texture tex;

// 0 = linear, 1 = sin, 2 = distance, 3 = bezier, 4 none
int interpolation = 0;

float spd = 0.05f;
float ticks = 0.0f;

int thisFrame = 0;
int previousFrame = 0;
int nextFrame = 0;

int thisIndex = 0;
int previousIndex = 0;
int nextIndex = 0;

std::vector<int> frameIndex;

void draw(Renderer *renderer);
void render(Renderer *renderer);

void drawShadows(Renderer* renderer);

void setAnim(float spd, std::vector<int> fi, bool immediate = false);

void setSpd(float newSpd);

bool finished();

void renderShadows(Renderer *renderer);

void tick(float s = -1.0f);

void loadAnim(std::string path, std::string texpath, int numFrames, std::vector<int> frameI = {});

void addFrame(Mesh *newFrame);

void resetFrames();

void clearFrames();

Anim() {
}
};

class Anims {
public:
Mesh animFrame;
Mesh templateFrame;     // newFrame += template - frame1; newFrame += template - frame2; newFrame += template;

Texture tex;
Phys phys;

std::vector<Anim> anims;
std::vector<bool> animRunning;

void draw(Renderer *renderer);
void render(Renderer *renderer);

void drawShadows(Renderer *renderer);
void renderShadows(Renderer *renderer);

void addAnim(std::string path, int numFrames, float spd, std::vector<int> frameI = {});
void loadTemplate(std::string path, std::string texpath);
void clearAnims();

void resetStates();

void tick();

Anims()
{
}
};

#endif //CORIUM_ANIM_H
