/**
 * @file globals.cpp
 * @brief Global variables and state for the neuron viewer application
 * 
 * This file contains the implementation of global variables used throughout
 * the neuron visualization application. These variables maintain the application
 * state, including view parameters, user interface state, and the current
 * neuron data being visualized.
 *
 * Key components:
 * - View parameters (rotation, zoom, pan)
 * - Rendering mode and quality settings
 * - User interaction state (mouse dragging, etc.)
 * - Help text and UI state
 * - Current neuron data and graph representation
 *
 * @note These variables are declared as extern in globals.h
 * @see globals.h for declarations and documentation of each variable
 * 
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 */

#include <string>
#include "globals.h"

/** @brief Current rendering mode (1-6) that determines how the neuron is displayed */
int renderMode = 1;
/** @brief Rotation around the X-axis in degrees */
double rotateX = 0.0f;

/** @brief Rotation around the Y-axis in degrees */
double rotateY = 0.0f;

/** @brief Zoom factor (1.0 = normal, >1.0 = zoom in, <1.0 = zoom out) */
double zoom = 1.0f;
/** @brief Horizontal pan offset in screen coordinates */
double panX = 0.0f;

/** @brief Vertical pan offset in screen coordinates */
double panY = 0.0f;
/** @brief Flag indicating if left mouse button is being dragged */
bool dragging = false;

/** @brief Flag indicating if right mouse button is being dragged */
bool rightDragging = false;
/** @brief X coordinate of the last mouse position */
double lastX;

/** @brief Y coordinate of the last mouse position */
double lastY;

/**
 * @brief Refinement parameter for geometry processing
 * 
 * Controls the level of detail in the generated geometry.
 * Higher values result in coarser meshes, while lower values
 * produce more detailed meshes. Used in cubic spline interpolation.
 * 
 * @note Initialized to 8.0 for a coarse starting point
 * @see allCubicSplineResampledTrunks()
 */
double refineDelta = 8.0;

/**
 * @brief Help text displayed in the console and help window
 * 
 * Contains formatted text describing the application's keyboard
 * and mouse controls. The text is displayed when the user presses
 * the 'H' key or when explicitly requested.
 * 
 * @see printHelpText()
 */
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

/**
 * @brief Global container for the currently loaded neuron nodes
 * 
 * This vector holds the SWC nodes of the currently loaded neuron.
 * It's used throughout the application for rendering and processing.
 * 
 * @note Updated when a new file is loaded via openNeuronFile()
 * @see SWCNode
 */
std::vector<SWCNode> currentNodes;

/**
 * @brief Pointer to the GLFW help window
 * 
 * This is the main window used for displaying the help text.
 * Initialized to nullptr and created when the help window is shown.
 * 
 * @note Managed by the main application loop
 * @see showHelpWindow() in the main application file
 */
GLFWwindow* helpWindow = nullptr;
/**
 * @brief Global instance of the neuron graph
 * 
 * This object holds the complete neuron graph structure and provides
 * methods for loading, processing, and querying the neuron morphology.
 * 
 * @see NeuronGraph class for available operations
 */
NeuronGraph graph;
