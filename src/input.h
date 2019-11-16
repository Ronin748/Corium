//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_INPUT_H
#define CORIUM_INPUT_H

#include "corium.h"

class Input {
public:
	void getKeys(GLFWwindow *window);
	void getMouse(GLFWwindow *window);

	std::vector<bool> keyDown;
	std::vector<bool> keyPressed;

	std::vector<bool> mouseDown;

	glm::dvec2 mousePos;
	glm::dvec2 mousePosOld;
	glm::dvec2 mousePosDiff;
	float mouseSpd;

	int jsnum = 0;     // joystick number

	bool winFocus;
	bool focused;

	bool fullscreen = false;

	int ticks;

	GLFWwindow* thisWindow;

	// TODO: control configs
	int UP = 0;
	int DOWN = 0;
	int RIGHT = 0;
	int LEFT = 0;

	bool getKey(int id);

	Input()
	{
        winFocus = true;
        focused = false;

        mouseSpd = 1.0;

        keyDown.resize(256);
        keyPressed.resize(256);
        mouseDown.resize(3);

        mousePos = glm::vec2(0.0f, 0.0f);

        ticks = 0;

        std::fill(keyDown.begin(), keyDown.end(), false);
        std::fill(keyPressed.begin(), keyPressed.end(), false);
        std::fill(mouseDown.begin(), mouseDown.end(), false);
	}
};

#endif //CORIUM_INPUT_H
