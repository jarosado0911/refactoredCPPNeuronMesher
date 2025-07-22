// call back functions

#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "neurongraph.h"
#include "globals.h"
#include "opgl_utils.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
