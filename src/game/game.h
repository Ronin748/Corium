//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_GAME_H
#define CORIUM_GAME_H

#include <audio.h>
#include <cstdio>
#include <ctime>

#include "corium.h"
#include "window.h"
#include "input.h"
#include "render/renderer.h"
#include "world/map.h"
#include "gui/screen.h"

class Game {
public:
	bool running = false;

	bool win = false;
	bool lose = false;
	bool reset = false;

	bool startScreen = false;

	Screen scr;

	Window wndw;
	Input input;

	Player *thisPlayer;
	Camera *thisCamera;

	Renderer renderer;

	Audio aud;

	Map map;

	int ticks = 0;
	int fps = 0;

	int frame_ticks = 0;
	int game_ticks = 0;

	bool ticking = false; // is game doing tick cycle?

	float renderTime = 0.0f;
	float tickTime = 0.0f;

	std::chrono::high_resolution_clock timer;
	std::chrono::system_clock::duration start_time;

	bool multithreading = true;
	bool loadShaders = true;

	bool fileoutput = true;

	FILE *outputfile = NULL;

	bool music = true;

	bool startFullScr = false;

	void init();

	void start();

	void refresh();

	void main();
	void gameRender();
	void rendererUpdate();
	void rendererReset();

	void gameInput();

	void loadGameCfg();

	int getTime();

	void tick();

	std::vector<std::string> text_buffer;

	void print(std::string s);

	void fullScr();

	void loadingScreen();

	void gameQuit();

	Game();
};

void loadSettings(std::string path, Game *game);

void runScript(std::string path, Game *game);

#endif //CORIUM_GAME_H
