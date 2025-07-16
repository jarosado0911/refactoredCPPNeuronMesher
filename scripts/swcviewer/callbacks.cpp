
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
            renderMode = key - GLFW_KEY_0;
            std::cout << "Switched to render mode " << renderMode << "\n";
        } else if (key == GLFW_KEY_O) {
            auto data = reinterpret_cast<std::tuple<std::vector<SWCNode>*, float*, float*, float*, float*, float*, float*, float*, float*, float*, float*>*>(glfwGetWindowUserPointer(window));
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
        rotateY += static_cast<float>(dx) * 0.3f;
        rotateX += static_cast<float>(dy) * 0.3f;
        lastX = xpos;
        lastY = ypos;
    } else if (rightDragging) {
        double dx = xpos - lastX;
        double dy = ypos - lastY;
        panX += static_cast<float>(dx) * 1.5f;
        panY -= static_cast<float>(dy) * 1.5f;
    }
    lastX = xpos;
    lastY = ypos;

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom *= std::pow(1.1f, yoffset);
    zoom = std::clamp(zoom, 0.05f, 10.0f);
}
