//
// Created by Roninkoi on 21.12.2015.
//

#include "game.h"

// update renderer values per cycle
void Game::rendererUpdate()
{
	renderer.w = window.current_w;
	renderer.h = window.current_h;

	renderer.camPos = thisCamera->pos;
	renderer.camRot = thisCamera->rot;
	renderer.camZ = thisCamera->z;
	renderer.fov = thisCamera->fov;

	renderer.batchesPerCycle = 0;
	renderer.drawsPerCycle = 0;
}

void Game::rendererReset()
{
	renderer.destroyRenderer();
	renderer.init();
	screen.guiRenderer.destroyRenderer();
	screen.guiRenderer.init();
	screen.guiRenderer.update(&renderer);
}

void Game::loadingScreen()
{
	screen.guiRenderer.update(&renderer);
	screen.drawLoadingScreen(&renderer);
	renderer.flushBatchFBO();
	window.update(&running);
}

void Game::gameRender()
{
	if (startScreen) {
		thisCamera->pos = thisCamera->camStart;
		thisCamera->rot.y = renderer.ticks / 100.0f;
		thisCamera->rot.x = 0.3f * sin(renderer.ticks / 100.0f);
	}

	rendererUpdate();

	renderer.update();

	map.draw(&renderer);

	// flush before screen draws to buffer
	renderer.flushBatch();

	// copy text buffer to screen
	screen.text_buffer.resize(textBuffer.size());
	for (int i = 0; i < textBuffer.size(); ++i) {
		screen.text_buffer[i] = textBuffer[i];
	}

	if (startScreen) {
		screen.drawStartScreen(&renderer);
	} else if (win) {
		screen.drawWinScreen(&renderer);
	} else {
		screen.drawScreen(&renderer);
	}

	// flush batch and render FBO if in use
	renderer.flushBatchFBO();

	renderer.batchesPerCycle += screen.guiRenderer.batchnum;
	renderer.drawsPerCycle += screen.guiRenderer.drawnum;

	renderer.lightNum = map.lightNum;
}
