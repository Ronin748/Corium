//
// Created by Roninkoi on 22.12.2015.
//

#ifndef CORIUM_GUI_H
#define CORIUM_GUI_H

#include "quad.h"
#include "render/guiRenderer.h"

class GUI {
public:
	Quad crosshair;

	Quad hurt;

	float ticks = 0.0f;

	void drawGUI(GUIRenderer *guiRenderer);

	void loadGUI();

	GUI()
	{
	}
};

#endif //CORIUM_GUI_H
