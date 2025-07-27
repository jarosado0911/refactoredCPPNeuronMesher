/**
 * @file globals.h
 * @brief Global state variables for the neuron visualization
 * 
 * This header declares all global variables used throughout the neuron viewer application.
 * These variables manage the visualization state, camera controls, and current neuron data.
 *
 * @warning Global variables should be used sparingly and only for truly global state
 * that needs to be accessed across multiple translation units.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 */

#pragma once
#include <vector>        // For std::vector
#include <string>        // For std::string
#include <GLFW/glfw3.h>  // For GLFWwindow

// Forward declarations
class NeuronGraph;
struct SWCNode;

/// @name Rendering State
///@{

/**
 * @brief Current rendering mode (1-5)
 * 
 * Controls how the neuron is rendered:
 * - 1: Basic line rendering
 * - 2: Cylinder rendering
 * - 3: Spheres at nodes
 * - 4: Full 3D rendering
 * - 5: Debug rendering mode
 */
extern int renderMode;

/**
 * @brief Camera rotation around X and Y axes (in degrees)
 * 
 * These angles control the 3D view orientation:
 * - rotateX: Rotation around the X-axis (vertical rotation)
 * - rotateY: Rotation around the Y-axis (horizontal rotation)
 */
extern double rotateX, rotateY;

/// @brief Camera zoom factor (higher values = more zoomed in)
extern double zoom;

/**
 * @brief Camera pan offset in X and Y directions
 * 
 * These values control the 2D panning of the view:
 * - panX: Horizontal pan (positive = right)
 * - panY: Vertical pan (positive = up)
 */
extern double panX, panY;

/**
 * @brief Mouse drag state flags
 * 
 * These track the current drag operation:
 * - dragging: True during left-button rotation
 * - rightDragging: True during right-button panning
 */
extern bool dragging, rightDragging;

/**
 * @brief Last recorded mouse cursor position
 * 
 * Used to calculate mouse movement deltas for smooth camera control.
 */
extern double lastX, lastY;

/// @brief Refinement parameter for mesh generation
extern double refineDelta;

/// @brief Help text displayed in the help window
extern std::string help;

/// @brief Currently loaded neuron nodes for rendering
extern std::vector<SWCNode> currentNodes;

/// @brief Pointer to the help window (nullptr if not open)
extern GLFWwindow* helpWindow;

/// @brief The main neuron graph data structure
extern NeuronGraph graph;

///@}
