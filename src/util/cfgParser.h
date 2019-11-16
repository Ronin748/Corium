//
// Created by Roninkoi on 22.12.2015.
//

#ifndef CORIUM_CFGPARSER_H
#define CORIUM_CFGPARSER_H

#include <game/game.h>
#include "coriumlang.h"

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
                if (expr(words, i, 2, "_SCREEN_SCALE")) {
                        game->renderer.SCREEN_SCALE = (float) atof(words[i + 1].c_str());

                        game->renderer.SCREEN_WIDTH = (GLuint) (512.0f * game->renderer.SCREEN_SCALE);
                        game->renderer.SCREEN_HEIGHT = (GLuint) (300.0f * game->renderer.SCREEN_SCALE);
                }
                if (expr(words, i, 2, "_SHADOW_SCALE")) {
                        float SHADOW_RES = (float) atof(words[i + 1].c_str());

                        game->renderer.SHADOW_WIDTH = (GLuint) (512.0f * SHADOW_RES);
                        game->renderer.SHADOW_HEIGHT = (GLuint) (512.0f * SHADOW_RES);
                }
                if (expr(words, i, 2, "_SHADOWS")) {
                        int SHADOW = (int) atoi(words[i + 1].c_str());

                        game->map.shadowsEnabled = (bool)SHADOW;
                }
                if (expr(words, i, 2, "_WINDOW_SCALE")) {
                        game->wndw.scale = (float) atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_SWAP_INTERVAL")) {
                        game->wndw.swapInterval = atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_MAX_LIGHTS")) {
                        game->renderer.MAX_LIGHTS = atoi(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_MULTITHREADING")) {
                        game->multithreading = (bool) atof(words[i + 1].c_str());
                        game->map.sys.multithreading = (bool) atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_LOADSHADERS")) {
                        game->loadShaders = (bool) atof(words[i + 1].c_str());

                        game->renderer.loadShaders = game->loadShaders;
                        game->scr.scrRndr.loadShaders = game->loadShaders;

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
                        game->fileoutput = (bool) atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_DRAWLINES")) {
                        game->renderer.drawLines = (bool) atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_FULLSCREEN")) {
                        game->startFullScr = (bool) atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_STARTSCREEN")) {
                        game->startScreen = (bool) atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_GLVER")) {
                        game->wndw.gl_maj = (int) atof(words[i + 1].c_str());
                        game->wndw.gl_min = (int) atof(words[i + 2].c_str());
                }
                if (expr(words, i, 2, "_CAMERA_AUTO")) {
                        game->thisCamera->camAI = (bool) atof(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_FRAMEBUFFER")) {
                        game->renderer.rfbo = (bool) atoi(words[i + 1].c_str());

                        if (game->renderer.rfbo) {
                                game->renderer.SCREEN_WIDTH = game->renderer.w;
                                game->renderer.SCREEN_HEIGHT = game->renderer.h;
                        }
                        else {
                                game->renderer.SCREEN_WIDTH = 256 * 2 * game->renderer.SCREEN_SCALE;
                                game->renderer.SCREEN_HEIGHT = 150 * 2 * game->renderer.SCREEN_SCALE;
                        }
                }
                if (expr(words, i, 2, "_DITHERING")) {
                        game->renderer.dithering = (bool) atoi(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_ANTIALIAS")) {
                        game->wndw.aa_samples = atoi(words[i + 1].c_str());
                }
                if (expr(words, i, 2, "_CONTROLS")) {
                        std::string controlsPath = words[i + 1].c_str();
                        //loadControls(controlsPath, &game->input);
                }
                if (expr(words, i, 2, "_RUN")) {
                        std::string scriptPath = words[i + 1].c_str();
                        runScript(scriptPath, game);
                }
        }
}

#endif //CORIUM_CFGPARSER_H
