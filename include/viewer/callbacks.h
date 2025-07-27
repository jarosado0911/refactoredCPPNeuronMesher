/**
 * @file callbacks.h
 * @brief GLFW input handling callbacks for neuron visualization
 * 
 * This header defines the callback functions that handle user input through GLFW,
 * including keyboard, mouse, and scroll wheel interactions for the neuron viewer.
 * These callbacks modify the global state in globals.h to control the visualization.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 */

#pragma once
#include <GLFW/glfw3.h>  // For GLFW window and input handling
#include <iostream>      // For std::cerr
#include <vector>        // For std::vector

// Forward declarations
struct GLFWwindow;

// External dependencies
#include "neurongraph.h"  // For NeuronGraph and SWCNode
#include "globals.h"      // For global state variables
#include "opgl_utils.h"   // For utility functions like loadSWC

/**
 * @brief Handles keyboard input events
 * 
 * Processes key presses and releases to control the visualization:
 * - WASD: Move camera left/right/forward/backward
 * - QE: Move camera up/down
 * - Arrow keys: Rotate view
 * - R: Reset view
 * - Space: Toggle auto-rotation
 * - 1-5: Switch between rendering modes
 * - +/-: Adjust rendering parameters
 * - ESC: Close window
 * - H: Toggle help text
 * 
 * @param window The GLFW window that received the event
 * @param key The keyboard key that was pressed or released
 * @param scancode The system-specific scancode of the key
 * @param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
 * @param mods Bit field describing which modifier keys were held down
 */
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
/**
 * @brief Handles mouse button input events
 * 
 * Processes mouse button presses and releases for:
 * - Left button: Start/end rotation
 * - Right button: Start/end panning
 * - Middle button: Start/end zooming
 * 
 * @param window The GLFW window that received the event
 * @param button The mouse button that was pressed or released
 * @param action GLFW_PRESS or GLFW_RELEASE
 * @param mods Bit field describing which modifier keys were held down
 */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
/**
 * @brief Handles mouse cursor movement
 * 
 * Updates camera orientation or position based on mouse movement when:
 * - Left button is held: Rotate view
 * - Right button is held: Pan view
 * - Middle button is held: Zoom view
 * 
 * @param window The GLFW window that received the event
 * @param xpos The new cursor x-coordinate, relative to the left edge of the content area
 * @param ypos The new cursor y-coordinate, relative to the top edge of the content area
 */
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
/**
 * @brief Handles mouse scroll events
 * 
 * Zooms the view in/out based on vertical scroll input.
 * 
 * @param window The GLFW window that received the event
 * @param xoffset The scroll offset along the x-axis (unused)
 * @param yoffset The scroll offset along the y-axis (positive = scroll up, negative = scroll down)
 */
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
