#include <game/world/map.h>
#include "lexer.h"

void mapCmd(std::vector<std::string> words, int i, Map *map)
{
	if (expr(words, i, 9, "_OBJECT")) {
		std::string s = words[i + 1];
		std::string p = words[i + 2];
		std::string t = words[i + 3];
		std::string pos = words[i + 4];
		std::string rot = words[i + 5];
		std::string scale = words[i + 6];
		std::string st = words[i + 7];
		std::string ph = words[i + 8];

		if (p == "NULL")
			p = "";

		map->addObj(s, p, t, vecParser(pos), vecParser(rot), vecParser(scale), boolParser(st),
					boolParser(ph));
	}
	if (expr(words, i, 3, "_LIGHT")) {
		std::string s = words[i + 1];
		std::string t = words[i + 2];
		map->lights.resize(map->lights.size() + 1);
		map->lights[map->lights.size() - 1].pos = vecParser(s);
		map->lights[map->lights.size() - 1].col = vecParser(t);
	}
	if (expr(words, i, 2, "_NAME")) {
		std::string s = words[i + 1];
		map->mapName = s;
	}
	if (expr(words, i, 2, "_FAR_PLANE")) {
		std::string s = words[i + 1];
		map->renderer->farPlane = atof(&s[0]);
	}
	if (expr(words, i, 5, "_SKY")) {
		std::string s = words[i + 1];
		std::string t = words[i + 2];
		std::string c = words[i + 3];
		std::string p = words[i + 4];

		map->sky.loadObject(t, s);
		map->sky.phys.s = glm::vec3(atof(&c[0]));
		map->sky.phys.isStatic = true;
		map->sky.rendered = true;
		map->sky.ro = false; // true
		map->skyplane = atof(&p[0]);
	}
	if (expr(words, i, 2, "_RENDER_DIST")) {
		std::string s = words[i + 1];
		map->renderer->renderDist = atof(&s[0]);
	}
	if (expr(words, i, 2, "_MASS")) {
		std::string s = words[i + 1];
		map->newobjmass = atof(&s[0]);
	}
	if (expr(words, i, 2, "_AMB")) {
		std::string s = words[i + 1];
		map->renderer->amb = atof(&s[0]);
	}
	if (expr(words, i, 2, "_LIT")) {
		std::string s = words[i + 1];
		map->renderer->lit = atof(&s[0]);
	}
	if (expr(words, i, 2, "_FOG_COL")) {
		std::string s = words[i + 1];
		map->renderer->clearCol = vecParser(s);
	}
	if (expr(words, i, 2, "_GRAVITY")) {
		std::string s = words[i + 1];
		map->gravity = vecParser(s);
	}
	if (expr(words, i, 2, "_CAM_START")) {
		std::string s = words[i + 1];
		map->camera->camStart = -vecParser(s);
	}
	if (expr(words, i, 2, "_PLAYER_START")) {
		std::string s = words[i + 1];
		std::string t = words[i + 2];
		map->thisPlayer->startPos = vecParser(s);
		map->thisPlayer->startRot = vecParser(t);
	}
	if (expr(words, i, 2, "_RUN")) {
		std::string scriptPath = words[i + 1].c_str();
		gameScript(scriptPath);
	}
}
