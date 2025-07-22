// globals
#include <string>
#include "globals.h"

int renderMode = 1;
double rotateX = 0.0f, rotateY = 0.0f, zoom = 1.0f;
double panX = 0.0f, panY = 0.0f;
bool dragging = false, rightDragging = false;
double lastX, lastY;

double refineDelta = 8.0; // start coarse

std::string help = "=== SWC Viewer Controls ===\n"
                   "1 - 6         | Change render mode\n"
                   "F             | Refine geometry (cubic spline)\n"
                   "Ctrl + F      | Coarsen geometry (cubic spline)\n"
                   "O             | Open SWC/UGX file\n"
                   "S             | Save to SWC/UGX file\n"
                   "R             | Reset camera view\n"
                   "H             | Help screen\n"
                   "Mouse Drag    | Rotate scene (Left Button)\n"
                   "Mouse Drag    | Pan scene (Right Button)\n"
                   "Scroll Wheel  | Zoom in/out\n"
                   "ESC           | Exit viewer";

std::vector<SWCNode> currentNodes;

GLFWwindow* helpWindow = nullptr;
NeuronGraph graph;
