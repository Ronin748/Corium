//
// Created by Roninkoi on 22.12.2015.
//

#ifndef CORIUM_CFGPARSER_H
#define CORIUM_CFGPARSER_H

#include <game/game.h>
#include "coriumLang.h"

void loadControls(std::string path, Input *input)
{
}

void loadSettings(std::string path, Game *game)
{
	gamePrint("loading " + path + "...\n");

	std::vector<std::string> words = fileLexer(path);

	for (int i = 0; i < words.size(); ++i) {
		if (expr(words, i, 2, "_FOV")) {
			game->thisCamera->fov = atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_MOUSE_SPD")) {
			game->input.mouseSpd = atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_JSNUM")) {
			game->input.jsnum = atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_SCREEN_WIDTH")) {
			game->renderer.SCREEN_WIDTH = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_SCREEN_HEIGHT")) {
			game->renderer.SCREEN_HEIGHT = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_SHADOW_WIDTH")) {
			game->renderer.SHADOW_WIDTH = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_SHADOW_HEIGHT")) {
			game->renderer.SHADOW_HEIGHT = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_SHADOWS")) {
			int SHADOW = (int) atoi(words[i + 1].c_str());

			game->map.shadowsEnabled = (bool) SHADOW;
		}
		if (expr(words, i, 2, "_WINDOW_WIDTH")) {
			game->window.window_w = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_WINDOW_HEIGHT")) {
			game->window.window_h = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_MONITOR_WIDTH")) {
			game->window.monitor_w = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_MONITOR_HEIGHT")) {
			game->window.monitor_h = (GLuint) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_SWAP_INTERVAL")) {
			game->window.swapInterval = atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_MAX_LIGHTS")) {
			game->renderer.MAX_LIGHTS = atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_MULTITHREADING")) {
			game->map.sys.multithreading = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_MUSIC")) {
			game->music = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_PHYSICS")) {
			game->map.physicsEnabled = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_SLEEPING")) {
			game->map.sys.fd = !((bool) atoi(words[i + 1].c_str()));
		}
		if (expr(words, i, 2, "_FILEOUTPUT")) {
			game->fileOutput = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_DRAWLINES")) {
			game->renderer.drawLines = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_FULLSCREEN")) {
			game->startFullscreen = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_STARTSCREEN")) {
			game->startScreen = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_GLVER")) {
			game->window.gl_maj = (int) atof(words[i + 1].c_str());
			game->window.gl_min = (int) atof(words[i + 2].c_str());
		}
		if (expr(words, i, 2, "_CAMERA_AUTO")) {
			game->thisCamera->camAI = (bool) atof(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_FRAMEBUFFER")) {
			game->renderer.rfbo = (bool) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_DITHERING")) {
			game->renderer.dithering = (bool) atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_ANTIALIAS")) {
			game->window.aa_samples = atoi(words[i + 1].c_str());
		}
		if (expr(words, i, 2, "_CONTROLS")) {
			std::string controlsPath = words[i + 1].c_str();
			loadControls(controlsPath, &game->input);
		}
		if (expr(words, i, 2, "_SCREENSHADER_VERT")) {
			game->renderer.screenShader.vertPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_SCREENSHADER_FRAG")) {
			game->renderer.screenShader.fragPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_GUISHADER_VERT")) {
			game->screen.guiRenderer.guiShader.vertPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_GUISHADER_FRAG")) {
			game->screen.guiRenderer.guiShader.fragPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_MAINSHADER_VERT")) {
			game->renderer.mainShader.vertPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_MAINSHADER_FRAG")) {
			game->renderer.mainShader.fragPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_DEPTHSHADER_VERT")) {
			game->renderer.depthShader.vertPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_DEPTHSHADER_GS")) {
			game->renderer.depthShader.gsPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_DEPTHSHADER_FRAG")) {
			game->renderer.depthShader.fragPath = words[i + 1].c_str();
		}
		if (expr(words, i, 2, "_RUN")) {
			std::string scriptPath = words[i + 1].c_str();
			runScript(scriptPath, game);
		}
	}
}

#endif //CORIUM_CFGPARSER_H
