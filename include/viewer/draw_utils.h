// draw utilities
#pragma once
#include <array>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <vector>
#include "neurongraph.h"
#include "globals.h"

void drawSimpleSphere(const SWCNode& node, double size = 0.5f);
void drawSimpleLine(const SWCNode& a, const SWCNode& b);
void drawSphere(const SWCNode& node, int slices = 6, int stacks = 6);
void drawCylinder(const SWCNode& a, const SWCNode& b, int segments = 6);
void drawBoundingBox(double minX, double maxX, double minY, double maxY, double minZ, double maxZ);
void setColorByType(int type);
void renderSWC(const std::vector<SWCNode>& nodes);

