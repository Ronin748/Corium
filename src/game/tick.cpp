//
// Created by Roninkoi on 19.12.2015.
//

#include "game.h"

void Game::tick()
{
	++ticks;
	++gameTicks;
	renderer.ticks += 1.0f;
	map.ticks += 1.0f;
	screen.ticks += 1.0f;

	gameInput();

	if (reset) {
		reset = false;
		win = false;
		lose = false;
	}

	map.update();

	screen.tick();
}
