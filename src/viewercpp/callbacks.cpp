// call back functions
#include "globals.h"
#include "callbacks.h"

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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom *= std::pow(1.1f, yoffset);
    zoom = std::clamp(static_cast<float>(zoom), 0.05f, 10.0f);
}
