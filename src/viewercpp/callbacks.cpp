/**
 * @file callbacks.cpp
 * @brief Implementation of GLFW callback functions for the neuron viewer
 * 
 * This file contains the implementation of various GLFW callback functions
 * that handle user input and interaction with the neuron visualization window.
 * It includes keyboard, mouse, and scroll wheel event handling for controlling
 * the 3D view and interacting with neuron data.
 *
 * Key features:
 * - Keyboard shortcuts for view manipulation (rotation, zoom, pan)
 * - Mouse interaction for 3D view control
 * - File I/O operations (open/save)
 * - Interactive refinement of neuron geometry
 * - Help system display
 *
 * The implementation uses GLFW for window management and input handling,
 * and integrates with the NeuronGraph class for neuron data manipulation.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 * @see https://github.com/yourusername/refactoredCPPNeuronMesher
 */

#include "globals.h"
#include "callbacks.h"

/**
 * @brief Handles keyboard input events for the neuron viewer
 * @param[in] window The GLFW window that received the event
 * @param[in] key The keyboard key that was pressed or released
 * @param[in] scancode The system-specific scancode of the key
 * @param[in] action The key action (GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT)
 * @param[in] mods Bit field describing which modifier keys were held down
 * 
 * This callback function processes keyboard input for controlling the neuron viewer.
 * It implements the following keyboard shortcuts:
 * - F: Refine neuron geometry (halve delta)
 * - Ctrl+F: Coarsen neuron geometry (double delta)
 * - 1-6: Switch between different render modes
 * - O: Open a neuron file dialog
 * - R: Reset camera view
 * - H: Show/hide help window
 * - S: Save the current neuron data to file
 *
 * The function modifies global state for view parameters and triggers updates
 * to the neuron visualization when geometry is refined or coarsened.
 *
 * @note The function only processes key press events (GLFW_PRESS)
 * @note Modifier keys (Ctrl, Shift, Alt) are checked using the mods parameter
 * @see GLFW key documentation: https://www.glfw.org/docs/latest/input_guide.html#input_key
 */
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_F) {
            auto sharedData = static_cast<std::tuple<
                std::vector<SWCNode>*,
                double*, double*, double*, double*, double*, double*,
                double*, double*, double*, double*>*>(glfwGetWindowUserPointer(window));

            auto& nodes = *std::get<0>(*sharedData);

            std::map<int, SWCNode> originalNodeMap;
            for (const auto& n : nodes)
                originalNodeMap[n.id] = n;

            //NeuronGraph graph;
            graph.setNodes(originalNodeMap);

            auto trunks = graph.getTrunks(false);

            if (mods & GLFW_MOD_CONTROL) {
                // Ctrl+F → double delta
                refineDelta = std::min(refineDelta * 2.0, 64.0);
                std::cout << "[Refinement] Coarsening with delta = " << refineDelta << "\n";
            } else {
                // F → halve delta
                refineDelta = std::max(refineDelta * 0.5, 0.2);
                std::cout << "[Refinement] Refining with delta = " << refineDelta << "\n";
            }

            auto refined = graph.allCubicSplineResampledTrunks(trunks, refineDelta);
            auto trunkParentMap = graph.getTrunkParentMap(originalNodeMap, trunks);
            auto assembled = graph.assembleTrunks(refined, trunkParentMap);

            nodes.clear();
            for (const auto& [_, n] : assembled)
                nodes.push_back(n);
            currentNodes = nodes;
            computeBounds(nodes,
                          *std::get<1>(*sharedData), *std::get<2>(*sharedData),
                          *std::get<3>(*sharedData), *std::get<4>(*sharedData),
                          *std::get<5>(*sharedData), *std::get<6>(*sharedData),
                          *std::get<7>(*sharedData), *std::get<8>(*sharedData),
                          *std::get<9>(*sharedData), *std::get<10>(*sharedData));

            std::cout << "[Done] Geometry updated. Current delta: " << refineDelta << "\n";
            graph.setNodes(assembled);
        }

        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
            renderMode = key - GLFW_KEY_0;
            std::cout << "Switched to render mode " << renderMode << "\n";
        } else if (key == GLFW_KEY_O) {
            auto data = reinterpret_cast<std::tuple<std::vector<SWCNode>*, double*, double*, double*, double*, double*, double*, double*, double*, double*, double*>*>(glfwGetWindowUserPointer(window));
            if (data) {
                auto& [nodes, minX, maxX, minY, maxY, minZ, maxZ, centerX, centerY, centerZ, radius] = *data;
                openNeuronFile(*nodes, *minX, *maxX, *minY, *maxY, *minZ, *maxZ, *centerX, *centerY, *centerZ, *radius);
            }
        } else if (key == GLFW_KEY_R) {
            std::cout << "Resetting camera view\n";
            rotateX = 0.0f;
            rotateY = 0.0f;
            zoom = 1.0f;
            panX = panY = 0.0f;
            std::cout << "Camera view reset.\n";
        } else if (key == GLFW_KEY_H) {
            printHelpText();
            if (!helpWindow || glfwWindowShouldClose(helpWindow)) {
                if (helpWindow) {
                    glfwDestroyWindow(helpWindow);
                    helpWindow = nullptr;
                }

                helpWindow = glfwCreateWindow(500, 300, "Help", nullptr, nullptr);
                if (!helpWindow) {
                    std::cerr << "Failed to create help window.\n";
                } else {
                    glfwMakeContextCurrent(helpWindow);
                    glfwSwapInterval(1);
                    std::cout << "[Help] Help window created.\n";
                }
            } else {
                std::cout << "[Help] Help window already open.\n";
            }
        } else if (key == GLFW_KEY_S) {
            const char* filters[] = { "*.swc", "*.ugx" };
            const char* savePath = tinyfd_saveFileDialog(
                "Save Neuron File",
                "output.swc",
                2,
                filters,
                "SWC or UGX files"
            );

            if (savePath) {
                std::string pathStr = savePath;
                if (pathStr.size() >= 4 && pathStr.compare(pathStr.size() - 4, 4, ".swc") == 0) {
                    graph.writeToFile(pathStr);
                } else if (pathStr.size() >= 4 && pathStr.compare(pathStr.size() - 4, 4, ".ugx") == 0) {
                    graph.writeToFileUGX(pathStr);
                } else {
                    tinyfd_messageBox("Unsupported Format",
                                      "Please use a .swc or .ugx extension.",
                                      "ok", "error", 1);
                }
            }
        }
    }
}

/**
 * @brief Handles mouse button events for 3D view manipulation
 * @param[in] window The GLFW window that received the event
 * @param[in] button The mouse button that was pressed or released
 * @param[in] action The button action (GLFW_PRESS or GLFW_RELEASE)
 * @param[in] mods Bit field describing which modifier keys were held down
 * 
 * This callback function processes mouse button events to enable interactive
 * 3D view manipulation. It tracks the following interactions:
 * - Left mouse button: Enables view rotation when dragged
 * - Right mouse button: Enables view panning when dragged
 *
 * The function updates global state variables (dragging, rightDragging) that
 * are used by cursorPosCallback to implement the actual view transformations.
 *
 * @note The function tracks both press and release events for smooth interaction
 * @note The cursor position is captured on button press for relative movement
 * @see cursorPosCallback() for the implementation of the actual view transformations
 * @see GLFW mouse button documentation: https://www.glfw.org/docs/latest/input_guide.html#input_mouse_button
 */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            dragging = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            dragging = false;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightDragging = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            rightDragging = false;
        }
    }
}

/**
 * @brief Handles mouse cursor movement for view manipulation
 * @param[in] window The GLFW window that received the event
 * @param[in] xpos The new x-coordinate of the cursor, in screen coordinates
 * @param[in] ypos The new y-coordinate of the cursor, in screen coordinates
 * 
 * This callback function processes mouse movement to implement interactive
 * 3D view manipulation. It provides the following functionality:
 * - Left-drag: Rotates the view around the X and Y axes
 * - Right-drag: Pans the view in the X and Y directions
 *
 * The function uses the difference between the current and last cursor
 * positions to calculate the amount of rotation or panning to apply.
 * The actual view transformation is applied in the main rendering loop
 * using the global state variables modified by this function.
 *
 * @note The function updates the lastX and lastY variables to track
 *       cursor movement between frames
 * @note Rotation and panning speeds are scaled by constant factors (0.3f and 1.5f)
 * @see mouseButtonCallback() for enabling/disabling rotation and panning
 * @see GLFW cursor position documentation: https://www.glfw.org/docs/latest/input_guide.html#cursor_pos
 */
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (dragging) {
        double dx = xpos - lastX;
        double dy = ypos - lastY;
        rotateY += static_cast<double>(dx) * 0.3f;
        rotateX += static_cast<double>(dy) * 0.3f;
        lastX = xpos;
        lastY = ypos;
    } else if (rightDragging) {
        double dx = xpos - lastX;
        double dy = ypos - lastY;
        panX += static_cast<double>(dx) * 1.5f;
        panY -= static_cast<double>(dy) * 1.5f;
    }
    lastX = xpos;
    lastY = ypos;

}

/**
 * @brief Handles mouse scroll wheel events for zooming the view
 * @param[in] window The GLFW window that received the event
 * @param[in] xoffset The scroll offset along the x-axis (unused in this implementation)
 * @param[in] yoffset The scroll offset along the y-axis (positive for scroll up, negative for scroll down)
 * 
 * This callback function processes mouse scroll wheel input to implement smooth zooming
 * of the 3D view. It modifies the global zoom factor based on the vertical scroll amount.
 *
 * The zoom factor is adjusted using an exponential scale (1.1^yoffset) to provide
 * natural-feeling zoom behavior. The zoom level is clamped between 0.05 and 10.0 to
 * prevent extreme zoom levels that might cause rendering issues or numerical instability.
 *
 * @note The xoffset parameter is currently unused but included for future compatibility
 * @note Zooming is centered on the current view (no focal point adjustment)
 * @note The 1.1 base value provides a good balance between precision and responsiveness
 * @see GLFW scroll documentation: https://www.glfw.org/docs/latest/input_guide.html#scrolling
 */
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom *= std::pow(1.1f, yoffset);
    zoom = std::clamp(static_cast<float>(zoom), 0.05f, 10.0f);
}
