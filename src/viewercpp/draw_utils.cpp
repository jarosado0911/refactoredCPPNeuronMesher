// draw utilities
#include <array>
#include "draw_utils.h"

void drawSimpleSphere(const SWCNode& node, double size) {
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

void drawSphere(const SWCNode& node, int slices, int stacks) {
    glPushMatrix();
    glTranslatef(node.x, node.y, node.z);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, node.radius, slices, stacks);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void drawCylinder(const SWCNode& a, const SWCNode& b, int segments) {
    double vx = b.x - a.x, vy = b.y - a.y, vz = b.z - a.z;
    double len = std::sqrt(vx * vx + vy * vy + vz * vz);
    if (len == 0.0f) return;

    double ax = 57.2957795f * std::acos(vz / len);
    if (vz < 0.0f) ax = -ax;
    double rx = -vy * vz, ry = vx * vz;

    glPushMatrix();
    glTranslatef(a.x, a.y, a.z);
    glRotatef(ax, rx, ry, 0.0f);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluCylinder(quad, a.radius, b.radius, len, segments, 1);
    gluDeleteQuadric(quad);

    glPopMatrix();
}

void drawBoundingBox(double minX, double maxX, double minY, double maxY, double minZ, double maxZ) {
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

void setColorByType(int type) {
    static std::map<int, std::array<double, 3>> colorMap = {
        {1, {1.0f, 0.0f, 0.0f}}, {2, {0.0f, 1.0f, 0.0f}}, {3, {0.0f, 0.0f, 1.0f}},
        {4, {1.0f, 1.0f, 0.0f}}, {5, {1.0f, 0.0f, 1.0f}}, {6, {0.0f, 1.0f, 1.0f}}, {7, {1.0f, 0.5f, 0.0f}}
    };
    auto it = colorMap.find(type);
    if (it != colorMap.end())
        glColor3f(it->second[0], it->second[1], it->second[2]);
    else
        glColor3f(1.0f, 1.0f, 1.0f);
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
        if (node.pid == -1) continue;
        auto it = std::find_if(nodes.begin(), nodes.end(), [&](const SWCNode& n) { return n.id == node.pid; });
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
