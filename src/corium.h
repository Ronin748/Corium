//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_H
#define CORIUM_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <chrono>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "util/toString.h"

#define CORIUM_VERSION "1.4"
#define CORIUM_YEAR "2019"

void gamePrint(std::string s);

void gameRefresh();

void gameScript(std::string s);

void gameDrawLine(glm::vec3 l0, glm::vec3 l1);

#endif //CORIUM_H
