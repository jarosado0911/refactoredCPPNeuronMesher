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

void drawSphere(const SWCNode& node, int slices = 6, int stacks = 6) {
    glPushMatrix();
    glTranslatef(node.x, node.y, node.z);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, node.radius, slices, stacks);
    gluDeleteQuadric(quad);
    glPopMatrix();
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
