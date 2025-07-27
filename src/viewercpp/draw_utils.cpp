/**
 * @file draw_utils.cpp
 * @brief Implementation of OpenGL rendering utilities for neuron visualization
 * 
 * This file contains utility functions for rendering 3D neuron structures using OpenGL.
 * It provides various primitives for drawing spheres, cylinders, and other geometric
 * shapes used to represent neuron morphologies in 3D space.
 *
 * Key features:
 * - Sphere and cylinder rendering for soma and neurites
 * - Bounding box visualization
 * - Color mapping for different neuron components
 * - Multiple rendering modes (wireframe, solid, etc.)
 * - Support for different levels of detail
 *
 * The implementation uses legacy OpenGL immediate mode (glBegin/glEnd) for simplicity
 * and compatibility with the existing visualization pipeline.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 * @see https://github.com/yourusername/refactoredCPPNeuronMesher
 */

 #include <array>
 #include "neurongraph.h"  // For SWCNode definition
 #include "globals.h"      // For renderMode and other globals
 #include "draw_utils.h"
 
 /**
  * @brief Renders a low-detail sphere at the specified node's position
  * @param[in] node The SWCNode containing the position for the sphere
  * @param[in] size The radius of the sphere
  * 
  * This function draws a simple sphere using OpenGL's GLU quadric primitives.
  * It's optimized for speed rather than visual quality, using a low number of
  * slices and stacks (8x8) for the sphere approximation.
  *
  * The function handles the OpenGL matrix stack appropriately, pushing the
  * current matrix before translation and popping it afterward to avoid affecting
  * subsequent rendering operations.
  *
  * @note The sphere is drawn with the current OpenGL color
  * @note Uses legacy OpenGL immediate mode rendering
  * @see drawSphere() for a higher-quality sphere rendering alternative
  * @see gluSphere() for details on the underlying GLU primitive
  */
 void drawSimpleSphere(const SWCNode& node, double size) {
     glPushMatrix();
     glTranslatef(node.x, node.y, node.z);
     GLUquadric* quad = gluNewQuadric();
     // Apply a scaling factor for soma nodes (type 1)
     //double radius = (node.type == 1) ? size * 0.3 : size;
     gluSphere(quad, size, 8, 8);
     gluDeleteQuadric(quad);
     glPopMatrix();
 }
 
 /**
  * @brief Draws a straight line between two nodes
  * @param[in] a The starting node of the line
  * @param[in] b The ending node of the line
  * 
  * This function renders a simple line segment between two nodes in 3D space
  * using OpenGL's immediate mode line rendering. The line is drawn with the
  * current OpenGL color and line width settings.
  *
  * The function is typically used for wireframe representations of neuron
  * morphologies or for debug visualization.
  *
  * @note The line is drawn with the current OpenGL color
  * @note Uses legacy OpenGL immediate mode rendering (glBegin/glEnd)
  * @note For more complex line rendering (e.g., with thickness), consider using
  *       geometry shaders or other modern OpenGL techniques
  */
 void drawSimpleLine(const SWCNode& a, const SWCNode& b) {
     glBegin(GL_LINES);
     glVertex3f(a.x, a.y, a.z);
     glVertex3f(b.x, b.y, b.z);
     glEnd();
 }
 
 /**
  * @brief Renders a detailed sphere at the specified node's position
  * @param[in] node The SWCNode containing the position and radius for the sphere
  * @param[in] slices The number of subdivisions around the Z axis (longitude)
  * @param[in] stacks The number of subdivisions along the Z axis (latitude)
  * 
  * This function draws a sphere using OpenGL's GLU quadric primitives with smooth
  * normal vectors for proper lighting calculations. The sphere's radius is taken
  * from the node's radius property, and its position is set to the node's coordinates.
  *
  * The function handles the OpenGL matrix stack appropriately and uses the current
  * OpenGL color for rendering. The resolution of the sphere can be controlled
  * through the slices and stacks parameters.
  *
  * @note The sphere is drawn with smooth shading (GLU_SMOOTH)
  * @note Uses legacy OpenGL immediate mode rendering
  * @note For high-quality rendering, use at least 16 slices and stacks
  * @see drawSimpleSphere() for a faster, lower-quality alternative
  * @see gluQuadricNormals() for details on normal generation
  */
 void drawSphere(const SWCNode& node, int slices, int stacks) {
     glPushMatrix();
     glTranslatef(node.x, node.y, node.z);
     GLUquadric* quad = gluNewQuadric();
     gluQuadricNormals(quad, GLU_SMOOTH);
     // Apply a scaling factor for soma nodes (type 1)
     //double radius = (node.type == 1) ? node.radius * 0.3 : node.radius;
     gluSphere(quad, node.radius, slices, stacks);
     gluDeleteQuadric(quad);
     glPopMatrix();
 }
 
 /**
  * @brief Renders a tapered cylinder between two nodes
  * @param[in] a The starting node of the cylinder
  * @param[in] b The ending node of the cylinder
  * @param[in] segments The number of sides around the cylinder
  * 
  * This function draws a tapered cylinder connecting two nodes in 3D space.
  * The cylinder's radius at each end is determined by the radius property of
  * the corresponding node, allowing for smooth tapering of neurites.
  *
  * The function handles the necessary coordinate system transformations to
  * orient the cylinder correctly between the two points in 3D space. It uses
  * OpenGL's matrix stack to ensure the transformations don't affect subsequent
  * rendering operations.
  *
  * @note The cylinder is drawn with smooth normals for proper lighting
  * @note If the nodes are coincident (distance = 0), the function returns early
  * @note The 57.2957795f constant converts radians to degrees (180/π)
  * @note Uses legacy OpenGL immediate mode rendering
  * @see gluCylinder() for details on the underlying GLU primitive
  */
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
 
 /**
  * @brief Renders a wireframe bounding box in 3D space
  * @param[in] minX The minimum X coordinate of the bounding box
  * @param[in] maxX The maximum X coordinate of the bounding box
  * @param[in] minY The minimum Y coordinate of the bounding box
  * @param[in] maxY The maximum Y coordinate of the bounding box
  * @param[in] minZ The minimum Z coordinate of the bounding box
  * @param[in] maxZ The maximum Z coordinate of the bounding box
  * 
  * This function draws a white wireframe box representing the spatial bounds
  * of the neuron morphology. The box is drawn using OpenGL line primitives,
  * with each edge of the box represented by a single line segment.
  *
  * The function sets the current OpenGL color to white before rendering and
  * uses immediate mode rendering (glBegin/glEnd). The box is axis-aligned
  * and defined by the provided min/max coordinates in each dimension.
  *
  * @note The function sets the color to white (1.0, 1.0, 1.0)
  * @note Uses legacy OpenGL immediate mode rendering
  * @note The box is drawn with GL_LINES primitive type
  * @see glBegin() for details on immediate mode rendering
  * @see glVertex3f() for vertex specification
  */
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
 
 /**
  * @brief Sets the OpenGL color based on SWC node type
  * @param[in] type The SWC node type identifier
  * 
  * This function maps standard SWC node types to specific colors for visualization.
  * The color mapping is as follows:
  * - Type 1: Red (soma)
  * - Type 2: Green (axon)
  * - Type 3: Blue (basal dendrite)
  * - Type 4: Yellow (apical dendrite)
  * - Type 5: Magenta
  * - Type 6: Cyan
  * - Type 7: Orange
  * - Default: White (for any other type)
  *
  * The function uses a static map for efficient type-to-color lookup and
  * modifies the current OpenGL color state.
  *
  * @note The function modifies the OpenGL current color state
  * @note The color map is static and shared across all instances
  * @note For custom color schemes, modify the colorMap variable
  * @see http://www.neuronland.org/NLMorphologyConverter/MorphologyFormats/SWC/Spec.html
  *      for SWC format specification
  */
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
 
 /**
  * @brief Renders a collection of SWC nodes as a 3D neuron morphology
  * @param[in] nodes A vector of SWCNode objects representing the neuron morphology
  * 
  * This function renders a complete neuron morphology from SWC node data using
  * different rendering modes controlled by the global `renderMode` variable.
  * The function handles both the rendering of nodes (soma, branch points, etc.)
  * and the connections between them (dendrites, axons).
  *
  * Supported render modes:
  * - Mode 1: White wireframe lines between nodes
  * - Mode 2: Colored spheres at nodes with white wireframe connections
  * - Mode 3: Yellow simple spheres at node positions
  * - Mode 4: Purple detailed spheres at node positions
  * - Mode 5: Colored tapered cylinders between nodes (no spheres)
  * - Mode 6: Colored detailed spheres with colored tapered cylinders
  *
  * The function first renders all nodes according to the current render mode,
  * then renders the connections between nodes. Node colors are determined by
  * their SWC type using the setColorByType() function.
  *
  * @note The function uses the global `renderMode` variable to determine rendering style
  * @note Parent-child relationships are determined by the node.pid field
  * @note The function skips rendering of invalid parent references (pid = -1)
  * @see setColorByType() for information about color mapping
  * @see drawSphere(), drawCylinder() for the actual rendering primitives
  */
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
 