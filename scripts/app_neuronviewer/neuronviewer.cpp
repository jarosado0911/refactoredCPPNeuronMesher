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

void renderHelpWindow(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Top-left origin projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);  // Y flipped for top-left origin

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    std::string help =
        "=== SWC Viewer Controls ===\n"
        "1 - 6         | Change render mode\n"
        "O             | Open SWC file\n"
        "R             | Reset camera view\n"
        "Mouse Drag    | Rotate scene (Left Button)\n"
        "Mouse Drag    | Pan scene (Right Button)\n"
        "Scroll Wheel  | Zoom in/out\n"
        "ESC           | Exit viewer";

    std::string fileInfo = "Current file: " + (currentLoadedFile.empty() ? "[none]" : currentLoadedFile);

    // Configurable float font size
    const float fontScale = 1.25f;  // Try 1.5f, 2.25f, etc.
    const int charHeight = 13;     // Pixel height of stb_easy_font chars
    const float lineHeight = charHeight * fontScale;
    const float padding = 20.0f;

    // Count lines in help text
    int numLines = std::count(help.begin(), help.end(), '\n') + 1;
    float textBlockHeight = numLines * lineHeight;
    float startY = (height - textBlockHeight) / 2.0f;

    static char buffer[500000];

    glPushMatrix();
    glScalef(fontScale, fontScale, 1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);

    float printX = padding / fontScale;
    float printY = startY / fontScale;
    int num_quads = stb_easy_font_print(printX, printY, (char*)help.c_str(), nullptr, buffer, sizeof(buffer));
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);

    float fileY = height - padding;
    int num_quads_file = stb_easy_font_print(printX, fileY / fontScale, (char*)fileInfo.c_str(), nullptr, buffer, sizeof(buffer));
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads_file * 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();

    glfwSwapBuffers(window);
}

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
