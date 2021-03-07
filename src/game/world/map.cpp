//
// Created by Roninkoi on 21.12.2015.
//

#include "map.h"
#include "util/mapParser.h"

bool Map::visible(Obj *object)
{
	bool behind = true;

	glm::vec3 lookVec = glm::vec3(0.0f);
	glm::mat4 lookDir = glm::mat4(1.0f);

	lookDir = glm::rotate(lookDir, camera->rot.y,
						  glm::vec3(0.0f, 1.0f, 0.0f));
	lookDir = glm::rotate(lookDir, camera->rot.x,
						  glm::vec3(cos(camera->rot.y), 0.0, sin(camera->rot.y)));

	lookVec = glm::vec3(glm::vec4(0.0f, 0.0f, -1.0f, 1.0f) * lookDir); // vector of player look dir?

	glm::vec3 camPos = camera->pos + lookVec * camera->z;

	glm::vec3 po = object->physMesh.bsCenter + camPos;

	po += (glm::normalize(-po) + glm::normalize(lookVec)) *
		  object->physMesh.bsRadius; // there's quite a bit of error (hack)

	float visAngle = glm::dot(glm::normalize(po), glm::normalize(lookVec));

	if (visAngle > 0.3f) // < 0.0f is behind
		behind = false;

	bool isVisible = inRange(object->physMesh.bsCenter,
							 object->physMesh.bsRadius) &&
					 (!behind || glm::length(-camPos - object->physMesh.bsCenter) < object->physMesh.bsRadius);

	object->visible = isVisible;

	return isVisible;
}

inline bool Map::inRange(Obj *object)
{
	bool ir = inRange(object->physMesh.bsCenter, object->physMesh.bsRadius);
	object->phys.inRange = ir;

	return ir;
}

inline bool Map::inRange(glm::vec3 bsc, float bsr)
{
	return (glm::length(-camera->pos - bsc) < bsr + renderer->farPlane * renderer->renderDist);
}

void Map::drawShadows(Renderer *renderer)
{
	for (auto &environ : environs) {
		if (environ.rendered && environ.shadows && inRange(&environ)) {
			environ.drawShadows(renderer);
		}
	}

	for (auto &obj : objs) {
		if (obj.rendered && obj.shadows && inRange(&obj) &&
			(shadowsEnabled || obj.phys.isStatic)) {
			obj.drawShadows(renderer);
		}
	}

	// GAME OBJECTS HERE

	if (shadowsEnabled) {
		thisPlayer->drawShadows(renderer);
	}

	renderer->flushShadows();
}

void Map::drawObjs(Renderer *renderer)
{
	for (auto &obj : objs)
		obj.draw(renderer);

	for (auto &environ : environs)
		environ.draw(renderer);

	// GAME OBJECTS HERE

	thisPlayer->draw(renderer);

	renderer->flushBatch();

	if (sky.rendered) {
		renderer->shadows = false;
		renderer->farPlane *= skyPlane;
		float ramb = renderer->amb;
		float rlght = renderer->lit;
		renderer->amb = 1.0f;
		renderer->lit = 0.0f;

		renderer->flushUpdate();
		sky.draw(renderer);
		renderer->flushBatch();

		renderer->farPlane /= skyPlane;
		renderer->amb = ramb;
		renderer->lit = rlght;
		renderer->shadows = true;
		renderer->flushUpdate();
	}
}

void Map::bakeShadows()
{
	renderer->lightNum = 1;

	for (auto &light : lights) {
		light.initShadows();

		renderer->lights[0] = light;

		renderer->clearShadows();

		drawShadows(renderer);

		light = renderer->lights[0];
	}

	renderer->lightNum = renderer->MAX_LIGHTS;
}

void Map::draw(Renderer *renderer)
{
	this->renderer = renderer;

	if (shadowsEnabled) {
		if (shadowRender) {
			renderer->clearShadows();

			shadowRender = false;
			drawShadows(renderer);
		}
	} else {
		if (!baked) {
			bakeShadows();
			baked = true;
		}
	}

	renderer->flushUpdate();

	drawObjs(renderer);
}

void Map::mapParser()
{
	lights.clear();
	sky.rendered = false;

	std::vector<std::string> words = fileLexer(mapPath);

	for (int i = 0; i < words.size() && *running; ++i) {
		mapCmd(words, i, this);
	}
}

void Map::update()
{
	for (auto &i : player) {
		i.update();
		i.ticks = ticks;
	}

	sky.phys.pos = thisPlayer->renderPos;
	sky.phys.rot.y += 0.005f;
	sky.update();

	for (auto &obj : objs)
		visible(&obj);

	for (auto &environ : environs)
		visible(&environ);

	// GAME OBJECTS here

	clearLights(renderer);
	findLights();

	if (shadowsEnabled) {
		shadowRender = true;
	}

	thisPlayer->tick();

	tickPhysics();
}

void Map::tickPhysics()
{
	sys.field = gravity;

	if (physicsEnabled && physAdded) {
		sys.update();
	}

	if (!thisPlayer->flying && physAdded) thisPlayer->collide(&sys);
}
