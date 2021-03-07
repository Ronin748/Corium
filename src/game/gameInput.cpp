//
// Created by Roninkoi on 20.12.2015.
//

#include "game.h"

int grabbed = 0;
bool jumped = true;
bool thrown = false;
bool rightPress = false;
float dist = 2.0f;

bool restartPressed = false;

bool flyingPressed = false;

bool console = true;

int meshNo = 0;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	dist += (float) (-yoffset / 10.0);
}

void Game::gameInput()
{
	glfwSetScrollCallback(window.window, scroll_callback);

	input.getKeys(window.window);
	input.getMouse(window.window);

	if (!startScreen && !win && !lose) {
		thisPlayer->getInput(&input);
	} else {
		thisPlayer->phys.v = glm::vec3(0.0f);
		thisPlayer->phys.rotV = glm::vec3(0.0f);
		thisPlayer->moving = false;
	}

	thisCamera->z = thisCamera->z * 0.8f + dist * 3.0f * 0.2f;

	if (glfwGetWindowAttrib(window.window, GLFW_FOCUSED) && glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_LEFT) &&
		!input.focused) {
		if (input.winFocus) {
			input.winFocus = false;
			input.focused = true;
			glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	} else {
		input.winFocus = true;
	}
	if (input.keyDown[GLFW_KEY_M]) {
		if (input.keyPressed[GLFW_KEY_M]) {
			if (audio.playingMusic()) {
				audio.stopMusic();
			} else {
				audio.playMusic();
			}
			input.keyPressed[GLFW_KEY_M] = false;
		}
	} else {
		input.keyPressed[GLFW_KEY_M] = true;
	}
	if (input.keyDown[GLFW_KEY_Y]) {
		if (flyingPressed) {
			thisPlayer->flying = !thisPlayer->flying;
			flyingPressed = false;
		}
	} else {
		flyingPressed = true;
	}

	if (glfwGetKey(window.window, GLFW_KEY_F11)) {
		if (input.fullscreen) {
			fullScr();

			input.fullscreen = false;
		}
	} else {
		input.fullscreen = true;
	}
	if (glfwGetKey(window.window, GLFW_KEY_F2)) {
		if (console) {
			console = false;
			screen.console = !screen.console;
		}
	} else if (glfwGetKey(window.window, GLFW_KEY_F3)) {
		if (console) {
			console = false;
			screen.big = !screen.big;
		}
	} else {
		console = true;
	}

	if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) && !window.fullscreen) {
		input.focused = false;
		glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (input.keyDown[GLFW_KEY_V] && !thrown) {
		map.sys.finish();
		map.objs[grabbed].phys.rotV = glm::vec3(0.0f);
		map.objs[grabbed].phys.pos = glm::vec3(thisPlayer->renderPos.x + dist * sin(-thisPlayer->phys.rot.y + M_PI),
											   thisPlayer->renderPos.y + dist * sin(thisPlayer->phys.rot.x + M_PI),
											   thisPlayer->renderPos.z + dist * cos(-thisPlayer->phys.rot.y + M_PI));
		map.objs[grabbed].phys.rot.y = -thisPlayer->phys.rot.y;
		map.objs[grabbed].phys.v = thisPlayer->phys.v - thisPlayer->phys.rotV + map.gravity;
	}

	int count;
	const unsigned char *jb = glfwGetJoystickButtons(input.jsnum, &count);
	bool jb1 = false;
	bool jb2 = false;
	bool jb3 = false;
	if (count >= 2) {
		jb1 = jb[1];
		jb2 = jb[0];
		jb3 = jb[2];
	}
	if (glfwGetKey(window.window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window.window, GLFW_KEY_RIGHT_SHIFT) || jb2) {
		thisPlayer->spd = 4.5f * 0.5f * 0.5f * 2.2f;
		if (thisPlayer->moving) thisPlayer->sprinting = true;
	} else {
		thisPlayer->spd = 4.5f * 0.5f * 0.5f;
		thisPlayer->sprinting = false;
	}
	if (jb3 || glfwGetKey(window.window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window.window, GLFW_KEY_RIGHT_CONTROL)) {
		thisPlayer->flaming = true;
	} else {
		thisPlayer->flaming = false;
	}

	if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_LEFT)) {
		if (input.keyDown[GLFW_KEY_V] && !thrown) {
			thrown = true;
			map.objs[grabbed].phys.pos = glm::vec3(thisPlayer->renderPos.x + dist * sin(-thisPlayer->phys.rot.y + M_PI),
												   thisPlayer->renderPos.y + dist * sin(thisPlayer->phys.rot.x + M_PI),
												   thisPlayer->renderPos.z +
												   dist * cos(-thisPlayer->phys.rot.y + M_PI));
			map.objs[grabbed].phys.rot.y = -thisPlayer->phys.rot.y;

			map.objs[grabbed].phys.v = map.gravity;
			map.objs[grabbed].phys.v +=
					glm::vec3(sin(-thisPlayer->phys.rot.y + M_PI),
							  sin(thisPlayer->phys.rot.x + M_PI),
							  cos(-thisPlayer->phys.rot.y + M_PI));
			map.objs[grabbed].update();
		}
	} else if (!input.keyDown[GLFW_KEY_V]) { thrown = false; }
	if (glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_RIGHT)) {
	}

	if ((input.getKey(GLFW_KEY_SPACE) || jb1) && thisPlayer->health > 0.0f) {
		if (!jumped) {
			if (startScreen)
				startScreen = false;
			if (glm::length(map.gravity) > 0.0f) {
				thisPlayer->phys.v += -map.gravity * 35.0f;
				//audio.playAudio(6, audio.jump, 0);
			}
		}
		jumped = true;
	} else if (thisPlayer->onGround) {
		jumped = false;
	}

	if (input.keyDown[GLFW_KEY_C] && ticks % 6 == 0) {
		if (input.keyPressed[GLFW_KEY_C] || true) {
			map.sys.finish();
			std::string path;
			switch (meshNo) {
				case 0:
					path = "gfx/models/tetra.obj";
					break;
				case 1:
				case 2:
					path = "gfx/models/cube.obj";
					break;
				case 3:
					path = "gfx/models/ico.obj";
					break;
				default:
					path = "gfx/models/tetra.obj";
					break;
			}
			++meshNo;
			if (meshNo > 3) meshNo = 0;

			map.addObj(path, "", "gfx/white.png",
					   glm::vec3(thisPlayer->phys.pos.x + 3.0f * sin(-thisPlayer->phys.rot.y + M_PI),
								 thisPlayer->phys.pos.y + 3.0f * sin(thisPlayer->phys.rot.x + M_PI),
								 thisPlayer->phys.pos.z + 3.0f * cos(-thisPlayer->phys.rot.y + M_PI)),
					   glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), false, true);
			map.objs[map.objs.size() - 1].phys.isStatic = false;
			if (meshNo == 2) {
				map.objs[map.objs.size() - 1].phys.s = glm::vec3(0.7f);
				map.objs[map.objs.size() - 1].phys.m = 0.3f;
			}
			if (meshNo == 1) map.objs[map.objs.size() - 1].phys.s = glm::vec3(1.3f);

			map.objs[map.objs.size() - 1].phys.s *= glm::vec3(1.8f);

			float r = random_range(0, 1000) / 1000.0f;
			float g = random_range(0, 1000) / 1000.0f;
			float b = random_range(0, 1000) / 1000.0f;

			for (int i = 0; i < map.objs[map.objs.size() - 1].mesh.colData.size(); i += 4) {
				map.objs[map.objs.size() - 1].mesh.colData[i] = r + random_range(0, 5) / 20.0f;
				map.objs[map.objs.size() - 1].mesh.colData[i + 1] = g + random_range(0, 5) / 20.0f;
				map.objs[map.objs.size() - 1].mesh.colData[i + 2] = b + random_range(0, 5) / 20.0f;
				map.objs[map.objs.size() - 1].mesh.colData[i + 3] = 1.0f;
			}
			map.objs[map.objs.size() - 1].phys.v = glm::vec3(dist * sin(-thisPlayer->phys.rot.y + M_PI),
															 dist * sin(thisPlayer->phys.rot.x + M_PI),
															 dist * cos(-thisPlayer->phys.rot.y + M_PI)) * 0.05f;
			input.keyPressed[GLFW_KEY_C] = false;
			map.physAdded = false;
		}
	} else {
		input.keyPressed[GLFW_KEY_C] = true;
		if (!map.physAdded) {
			map.addPhysObjects();
			map.physAdded = true;
		}
	}

	if (glfwGetKey(window.window, GLFW_KEY_RIGHT_CONTROL)) {
		if (!rightPress && grabbed < map.objs.size() - 1) {
			rightPress = true;
			grabbed += 1;
			print("grabbed " + toString(grabbed) + "\n");
		}
	} else if (glfwGetKey(window.window, GLFW_KEY_LEFT_CONTROL)) {
		if (!rightPress && grabbed > 0) {
			rightPress = true;
			grabbed -= 1;
			print("grabbed " + toString(grabbed) + "\n");
		}
	} else {
		rightPress = false;
	}

	if (input.keyDown[GLFW_KEY_I]) {
		loadGameCfg();
		loadingScreen();
		map.reload(&renderer);
		grabbed = map.objs.size() - 1;
	}

	if (input.keyDown[GLFW_KEY_U]) {
		if (restartPressed) {
			restartPressed = false;
			rendererReset();
		}
	}
	if (input.keyDown[GLFW_KEY_O]) {
		loadGameCfg();
	}
	if (input.keyDown[GLFW_KEY_T]) {
		if (restartPressed) {
			restartPressed = false;
			map.restart();
		}
	} else {
		restartPressed = true;
	}
}
