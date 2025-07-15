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

int renderMode = 1;

float rotateX = 0.0f, rotateY = 0.0f, zoom = 1.0f;
bool dragging = false;
double lastX, lastY;

struct SWCNode {
    int id, type, parent;
    float x, y, z, radius;
};

std::vector<SWCNode> loadSWC(const std::string& filename) {
    std::vector<SWCNode> nodes;
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening SWC file: " << filename << "\n";
        return nodes;
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        SWCNode node;
        if (!(iss >> node.id >> node.type >> node.x >> node.y >> node.z >> node.radius >> node.parent)) {
            std::cerr << "Malformed line: " << line << "\n";
            continue;
        }
        nodes.push_back(node);
    }
    return nodes;
}

void setColorByType(int type) {
    static std::map<int, std::array<float, 3>> colorMap = {
        {1, {1.0f, 0.0f, 0.0f}}, {2, {0.0f, 1.0f, 0.0f}}, {3, {0.0f, 0.0f, 1.0f}},
        {4, {1.0f, 1.0f, 0.0f}}, {5, {1.0f, 0.0f, 1.0f}}, {6, {0.0f, 1.0f, 1.0f}}, {7, {1.0f, 0.5f, 0.0f}}
    };
    auto it = colorMap.find(type);
    if (it != colorMap.end())
        glColor3f(it->second[0], it->second[1], it->second[2]);
    else
        glColor3f(1.0f, 1.0f, 1.0f);
}

void drawSphere(const SWCNode& node, int slices = 6, int stacks = 6) {
    glPushMatrix();
    glTranslatef(node.x, node.y, node.z);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, node.radius, slices, stacks);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void drawSimpleSphere(const SWCNode& node, float size = 0.5f) {
    glPushMatrix();
    glTranslatef(node.x, node.y, node.z);
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, size, 8, 8);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void drawSimpleLine(const SWCNode& a, const SWCNode& b) {
    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
}

void drawCylinder(const SWCNode& a, const SWCNode& b, int segments = 6) {
    float vx = b.x - a.x, vy = b.y - a.y, vz = b.z - a.z;
    float len = std::sqrt(vx * vx + vy * vy + vz * vz);
    if (len == 0.0f) return;

    float ax = 57.2957795f * std::acos(vz / len);
    if (vz < 0.0f) ax = -ax;
    float rx = -vy * vz, ry = vx * vz;

    glPushMatrix();
    glTranslatef(a.x, a.y, a.z);
    glRotatef(ax, rx, ry, 0.0f);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluCylinder(quad, a.radius, b.radius, len, segments, 1);
    gluDeleteQuadric(quad);

    glPopMatrix();
}

void drawBoundingBox(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(minX, minY, minZ); glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, minY, minZ); glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, minY, maxZ); glVertex3f(minX, minY, maxZ);
    glVertex3f(minX, minY, maxZ); glVertex3f(minX, minY, minZ);
    glVertex3f(minX, maxY, minZ); glVertex3f(maxX, maxY, minZ);
    glVertex3f(maxX, maxY, minZ); glVertex3f(maxX, maxY, maxZ);
    glVertex3f(maxX, maxY, maxZ); glVertex3f(minX, maxY, maxZ);
    glVertex3f(minX, maxY, maxZ); glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, minY, minZ); glVertex3f(minX, maxY, minZ);
    glVertex3f(maxX, minY, minZ); glVertex3f(maxX, maxY, minZ);
    glVertex3f(maxX, minY, maxZ); glVertex3f(maxX, maxY, maxZ);
    glVertex3f(minX, minY, maxZ); glVertex3f(minX, maxY, maxZ);
    glEnd();
}

void renderSWC(const std::vector<SWCNode>& nodes) {
    for (const auto& node : nodes) {
        if (renderMode == 3) {
            glColor3f(1, 0.8f, 0);
            drawSimpleSphere(node);
        } else if (renderMode == 4) {
            glColor3f(0.6f, 0.4f, 1.0f);
            drawSphere(node);
        } else if (renderMode == 2 || renderMode == 6) {
            setColorByType(node.type);
            drawSphere(node);
        }
    }
    for (const auto& node : nodes) {
        if (node.parent == -1) continue;
        auto it = std::find_if(nodes.begin(), nodes.end(), [&](const SWCNode& n) { return n.id == node.parent; });
        if (it != nodes.end()) {
            if (renderMode == 1 || renderMode == 2) {
                glColor3f(1, 1, 1);
                drawSimpleLine(*it, node);
            } else if (renderMode == 5 || renderMode == 6) {
                setColorByType(node.type);
                drawCylinder(*it, node);
            }
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
            renderMode = key - GLFW_KEY_0;
            std::cout << "Switched to render mode " << renderMode << "\n";
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
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoom *= std::pow(1.1f, yoffset);
    zoom = std::clamp(zoom, 0.05f, 10.0f);
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

void setupCamera(float cx, float cy, float cz, float radius, int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, static_cast<float>(width) / height, 0.1, 10000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cx, cy, cz + radius * 2.0f / zoom, cx, cy, cz, 0, 1, 0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./swc_viewer path_to_file.swc\n";
        return 1;
    }

    std::vector<SWCNode> nodes = loadSWC(argv[1]);
    if (nodes.empty()) return 1;
    std::cout << "Loaded " << nodes.size() << " nodes.\n";

    float minX = nodes[0].x, maxX = nodes[0].x;
    float minY = nodes[0].y, maxY = nodes[0].y;
    float minZ = nodes[0].z, maxZ = nodes[0].z;
    for (const auto& n : nodes) {
        minX = std::min(minX, n.x); maxX = std::max(maxX, n.x);
        minY = std::min(minY, n.y); maxY = std::max(maxY, n.y);
        minZ = std::min(minZ, n.z); maxZ = std::max(maxZ, n.z);
    }

    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float centerZ = (minZ + maxZ) / 2.0f;
    float radius = std::max({ maxX - minX, maxY - minY, maxZ - minZ });

    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 800, "SWC Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
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
        glTranslatef(centerX, centerY, centerZ);
        glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
        glTranslatef(-centerX, -centerY, -centerZ);

        drawBoundingBox(minX, maxX, minY, maxY, minZ, maxZ);
        renderSWC(nodes);

        glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
