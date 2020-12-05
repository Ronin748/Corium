//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_WINDOW_H
#define CORIUM_WINDOW_H

#include "corium.h"

class Window {
public:
	GLFWwindow *window;
	float swapInterval = 0;

	int gl_maj = 3;
	int gl_min = 1;

	int current_w = 0;
	int current_h = 0;

	int aa_samples = 0;

	int window_w = 1600, window_h = 900;
	int monitor_w = 1920, monitor_h = 1080;

	bool fullscreen = false;

	const GLFWvidmode *mode;
	GLFWmonitor *monitor;

	void createWindow()
	{
		glfwWindowHint(GLFW_SAMPLES, aa_samples);
		glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_maj);//4
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_min);//5

		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		monitor = glfwGetPrimaryMonitor();

		window = glfwCreateWindow(window_w, window_h, "Corium", fullscreen ? monitor : NULL, NULL);

		glfwMakeContextCurrent(window);

		mode = glfwGetVideoMode(monitor);
		//glfwSetWindowPos(m_Window, (mode->width - monitor_w*scale) / 2, (mode->height - monitor_h*scale) / 2);

		glfwSetWindowMonitor(window, fullscreen ? monitor : nullptr, fabs(mode->width - window_w) / 2.0f,
							 fabs(mode->height - window_h) / 2.0f, window_w, window_h, GLFW_DONT_CARE);

		fullscreen ? resize(monitor_w, monitor_h) : resize(window_w, window_h);
	}

	void destroyWindow()
	{
		glfwDestroyWindow(window);
	}

	void fullScr()
	{
		fullscreen = !fullscreen;
		glfwSetWindowMonitor(window, fullscreen ? monitor : nullptr, fabs(mode->width - window_w) / 2.0f,
							 fabs(mode->height - window_h) / 2.0f, monitor_w, monitor_h, GLFW_DONT_CARE);
		//glfwSetWindowMonitor(window, fullscreen ? monitor : NULL, 0, 0, 1920, 1080, GLFW_DONT_CARE);

		if (!fullscreen)
			resize(window_w, window_h);
	}

	void resize(int new_w, int new_h)
	{
		current_w = new_w;
		current_h = new_h;
		glfwSetWindowSize(window, current_w, current_h);
		glfwSetWindowPos(window, fabs(mode->width - window_w) / 2.0f,
						 fabs(mode->height - window_h) / 2.0f);
	}

	void update(bool *running)
	{
		glfwSwapInterval((int) swapInterval);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwWindowShouldClose(window) != 0) {
			if (running != nullptr) *running = false;
		}

		glfwGetWindowSize(window, &current_w, &current_h);
	}

	Window()
	{

	}
};

#endif //CORIUM_WINDOW_H
