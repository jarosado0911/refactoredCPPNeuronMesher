// globals for neuron viewer
#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include "neurongraph.h"

extern int renderMode;
extern double rotateX, rotateY, zoom, panX, panY;
extern bool dragging, rightDragging;
extern double lastX, lastY;
extern double refineDelta;
extern std::string help;
extern std::vector<SWCNode> currentNodes;
extern GLFWwindow* helpWindow;
extern NeuronGraph graph;
