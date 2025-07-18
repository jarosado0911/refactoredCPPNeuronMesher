#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include <array>
#include <tuple>

#include "tinyfiledialogs.h"
#include "stb_easy_font.h"

#include "neurongraph.h"
#include "draw_utils.h"
#include "callbacks.h"
#include "opgl_utils.h"
#include "globals.h"

#include "renderhelpwindow.cpp"

void setupLighting() {
    GLfloat light_position[] = { 1.0f, 1.0f, 2.0f, 0.0f };
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
}

void setupCamera(double cx, double cy, double cz, double radius, int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, static_cast<double>(width) / height, 0.1, 10000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cx, cy, cz + radius * 2.0f / zoom, cx, cy, cz, 0, 1, 0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./swc_viewer path_to_file.swc\n";
        return 1;
    }

    static std::vector<SWCNode> nodes = loadSWC(argv[1]);
    currentLoadedFile = argv[1];
    static double minX, maxX, minY, maxY, minZ, maxZ, centerX, centerY, centerZ, radius;
    computeBounds(nodes, minX, maxX, minY, maxY, minZ, maxZ, centerX, centerY, centerZ, radius);

    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 800, "SWC Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    GLFWwindow* helpWindow = glfwCreateWindow(500, 300, "Help", nullptr, nullptr);
    if (!helpWindow) {
        std::cerr << "Failed to create help window.\n";
    } else {
        glfwMakeContextCurrent(helpWindow);
        glfwSwapInterval(1);
    }

    glfwMakeContextCurrent(window);
    static auto sharedData = std::make_tuple(&nodes, &minX, &maxX, &minY, &maxY, &minZ, &maxZ, &centerX, &centerY, &centerZ, &radius);
    glfwSetWindowUserPointer(window, &sharedData);

    glfwSwapInterval(1);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    setupLighting();

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setupCamera(centerX, centerY, centerZ, radius, width, height);

        glPushMatrix();
        glTranslatef(panX, panY, -5.0f / zoom);

        glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
        glTranslatef(-centerX, -centerY, -centerZ);

        drawBoundingBox(minX, maxX, minY, maxY, minZ, maxZ);
        renderSWC(nodes);

        glPopMatrix();
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);

        // Render help viewer safely
        if (helpWindow) {
            if (!glfwWindowShouldClose(helpWindow)) {
                glfwMakeContextCurrent(helpWindow);
                renderHelpWindow(helpWindow);
                glfwMakeContextCurrent(window); // restore
            } else {
                glfwDestroyWindow(helpWindow);
                helpWindow = nullptr;
                glfwMakeContextCurrent(window); // restore
            }
        }

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
