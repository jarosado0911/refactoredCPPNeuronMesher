/**
 * @file draw_utils.h
 * @brief OpenGL drawing utilities for neuron visualization
 * 
 * This header provides functions for rendering 3D neuron morphologies using OpenGL.
 * It includes utilities for drawing spheres, cylinders, and other geometric primitives
 * that make up neuron visualizations.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 */

#pragma once
#include <array>        // For std::array
#include <vector>       // For std::vector
#include <GLFW/glfw3.h> // For OpenGL and GLFW functions
#include <GL/glu.h>     // For GLU utilities

// Forward declarations
struct SWCNode;

/**
 * @brief Draws a simple low-polygon sphere at a node's position
 * @param node The SWC node containing position and radius information
 * @param size Scale factor for the sphere radius (default: 0.5)
 * 
 * This function renders a sphere using immediate mode OpenGL with minimal
 * geometric detail, making it suitable for large numbers of nodes.
 */
void drawSimpleSphere(const SWCNode& node, double size = 0.5f);

/**
 * @brief Draws a simple line between two nodes
 * @param a First node (start point)
 * @param b Second node (end point)
 * 
 * Renders a line in 3D space between two nodes using immediate mode OpenGL.
 * The line width is determined by the nodes' radii.
 */
void drawSimpleLine(const SWCNode& a, const SWCNode& b);

/**
 * @brief Draws a sphere with specified geometric detail
 * @param node The SWC node to draw
 * @param slices Number of subdivisions around the z-axis (longitude) [default: 6]
 * @param stacks Number of subdivisions along the z-axis (latitude) [default: 6]
 * 
 * Uses GLU quadric objects to render a sphere with the specified level of detail.
 * The sphere is colored according to the node's type.
 */
void drawSphere(const SWCNode& node, int slices = 6, int stacks = 6);

/**
 * @brief Draws a cylinder connecting two nodes
 * @param a First node (start of cylinder)
 * @param b Second node (end of cylinder)
 * @param segments Number of radial segments for the cylinder [default: 6]
 * 
 * Renders a tapered cylinder that connects two nodes, with the radius at each
 * end determined by the nodes' radii. The cylinder is colored according to
 * the first node's type.
 */
void drawCylinder(const SWCNode& a, const SWCNode& b, int segments = 6);

/**
 * @brief Draws an axis-aligned bounding box
 * @param minX Minimum x-coordinate
 * @param maxX Maximum x-coordinate
 * @param minY Minimum y-coordinate
 * @param maxY Maximum y-coordinate
 * @param minZ Minimum z-coordinate
 * @param maxZ Maximum z-coordinate
 * 
 * Renders a wireframe box representing the spatial bounds of the neuron.
 * Uses immediate mode OpenGL line drawing.
 */
void drawBoundingBox(double minX, double maxX, double minY, double maxY, double minZ, double maxZ);

/**
 * @brief Sets the OpenGL color based on SWC node type
 * @param type SWC node type identifier (1-7)
 * 
 * Maps standard SWC node types to distinct colors for visualization.
 * The color mapping follows the standard SWC color scheme:
 * - 1 (Soma): Red
 * - 2 (Axon): Blue
 * - 3 (Dendrite): Yellow
 * - 4 (Apical dendrite): Purple
 * - 5-7: Other standard SWC types
 */
void setColorByType(int type);

/**
 * @brief Renders a complete SWC neuron morphology
 * @param nodes Vector of SWC nodes to render
 * 
 * This is the main rendering function that draws an entire neuron morphology.
 * It handles the display of all nodes and connections between them, with
 * appropriate coloring based on node types. The function respects the
 * current rendering mode and visualization settings from globals.h.
 */
void renderSWC(const std::vector<SWCNode>& nodes);

