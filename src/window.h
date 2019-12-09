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

int w = 0;
int h = 0;

float scale = 1.0f;

int aa_samples = 0;

int window_w = 1600, window_h = 900;
int screen_w = 1920, screen_h = 1080;

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
        //  glfwSetWindowPos(m_Window, (mode->width - screen_w*scale) / 2, (mode->height - screen_h*scale) / 2);

        glfwSetWindowMonitor(window, fullscreen ? monitor : NULL, fabs(mode->width - window_w*scale) / 2.0f, fabs(mode->height - window_h*scale) / 2.0f, window_w, window_h, GLFW_DONT_CARE);

        fullscreen ? resize(screen_w*scale, screen_h*scale) : resize(window_w*scale, window_h*scale);
}

void destroyWindow()
{
        glfwDestroyWindow(window);
}

void fullScr()
{
        fullscreen = !fullscreen;
        glfwSetWindowMonitor(window, fullscreen ? monitor : NULL, fabs(mode->width - window_w*scale) / 2.0f, fabs(mode->height - window_h*scale) / 2.0f, 1920, 1080, GLFW_DONT_CARE);
        //glfwSetWindowMonitor(window, fullscreen ? monitor : NULL, 0, 0, 1920, 1080, GLFW_DONT_CARE);

        if (!fullscreen)
                resize(window_w*scale, window_h*scale);
}

void resize(int new_w, int new_h)
{
        w = new_w;
        h = new_h;
        glfwSetWindowSize(window, w, h);
        glfwSetWindowPos(window, fabs(mode->width - window_w*scale) / 2.0f, fabs(mode->height - window_h*scale) / 2.0f);
}

void update(bool *running)
{
        glfwSwapInterval((int)swapInterval);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwWindowShouldClose(window) != 0) {
                if (running != NULL) *running = false;
        }

        glfwGetWindowSize(window, &w, &h);
}

Window()
{

}
};

#endif //CORIUM_WINDOW_H
