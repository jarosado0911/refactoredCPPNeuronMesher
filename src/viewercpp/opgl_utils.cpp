/**
 * @file opgl_utils.cpp
 * @brief OpenGL utility functions for neuron visualization
 * 
 * This file contains utility functions for handling OpenGL-related operations
 * in the neuron visualization application. It provides functionality for
 * loading neuron data from files, computing spatial bounds, and managing
 * the visualization state.
 *
 * Key features:
 * - Loading and parsing SWC/UGX neuron files
 * - Computing spatial bounds and center of neuron morphologies
 * - Managing file I/O for neuron data
 * - Integration with the main visualization pipeline
 *
 * The implementation handles various neuron file formats and provides
 * utility functions to support the OpenGL rendering pipeline.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 * @see globals.h for shared state variables
 * @see opgl_utils.h for function declarations
 */

#include "opgl_utils.h"
#include "globals.h"

/** @brief Path to the currently loaded neuron file */
std::string currentLoadedFile = ".";

/**
 * @brief Prints the help text to the standard output
 * 
 * Displays the application's keyboard and mouse controls to the console.
 * The help text is defined as a global string constant in globals.h.
 * 
 * @note The help text includes controls for view manipulation,
 *       file operations, and rendering modes.
 * @see globals.h for the help text definition
 */
void printHelpText() {
    std::cout << help;
}

/**
 * @brief Loads a neuron morphology from an SWC or UGX file
 * @param[in] filename Path to the SWC or UGX file to load
 * @return std::vector<SWCNode> Vector of SWC nodes representing the neuron
 * 
 * This function loads a neuron morphology from the specified file and returns
 * its nodes as a vector of SWCNode objects. It supports both SWC and UGX file
 * formats and performs preprocessing on the loaded data.
 *
 * The function:
 * 1. Reads the neuron data using NeuronGraph's readFromFileUGXorSWC()
 * 2. Applies preprocessing to the loaded nodes
 * 3. Converts the node map to a vector for rendering
 *
 * @note The function modifies the global graph object
 * @note Preprocessing includes validation and cleanup of the neuron structure
 * @throws std::runtime_error if the file cannot be read or parsed
 * @see NeuronGraph::readFromFileUGXorSWC()
 * @see NeuronGraph::preprocess()
 */
std::vector<SWCNode> loadSWC(const std::string& filename) {
    std::vector<SWCNode> nodes;
    //NeuronGraph graph;
    graph.readFromFileUGXorSWC(filename);
    auto prenodes = graph.getNodes();
    prenodes = graph.preprocess(prenodes);
    graph.setNodes(prenodes);

    std::cout << graph.getNodes().size() << std::endl;

    for(auto& [_,node] : graph.getNodes())
    {
        nodes.push_back(node);
        //std::cout << node.id << ' ' << node.pid << std::endl;
    }

    return nodes;
}

/**
 * @brief Computes the bounding box and center of a set of nodes
 * @param[in] nodes Vector of SWC nodes to compute bounds for
 * @param[out] minX Minimum X coordinate of the bounding box
 * @param[out] maxX Maximum X coordinate of the bounding box
 * @param[out] minY Minimum Y coordinate of the bounding box
 * @param[out] maxY Maximum Y coordinate of the bounding box
 * @param[out] minZ Minimum Z coordinate of the bounding box
 * @param[out] maxZ Maximum Z coordinate of the bounding box
 * @param[out] centerX X coordinate of the bounding box center
 * @param[out] centerY Y coordinate of the bounding box center
 * @param[out] centerZ Z coordinate of the bounding box center
 * @param[out] radius Radius of the bounding sphere (half the maximum dimension)
 * 
 * This function calculates the axis-aligned bounding box, center point, and
 * bounding sphere radius for a set of 3D points represented by SWC nodes.
 * The results are returned through reference parameters.
 *
 * The function also prints the computed bounds and center to the console
 * for debugging purposes.
 *
 * @note If the input vector is empty, the function returns immediately
 * @note The radius is defined as half the maximum dimension of the bounding box
 * @see openNeuronFile() for a typical usage example
 */
void computeBounds(const std::vector<SWCNode>& nodes, double& minX, double& maxX, double& minY, double& maxY, double& minZ, double& maxZ, double& centerX, double& centerY, double& centerZ, double& radius) {
    if (nodes.empty()) return;

    minX = maxX = nodes[0].x;
    minY = maxY = nodes[0].y;
    minZ = maxZ = nodes[0].z;

    for (const auto& n : nodes) {
        minX = std::min(minX, n.x); maxX = std::max(maxX, n.x);
        minY = std::min(minY, n.y); maxY = std::max(maxY, n.y);
        minZ = std::min(minZ, n.z); maxZ = std::max(maxZ, n.z);
    }

    centerX = (minX + maxX) / 2.0f;
    centerY = (minY + maxY) / 2.0f;
    centerZ = (minZ + maxZ) / 2.0f;
    radius = std::max({ maxX - minX, maxY - minY, maxZ - minZ });

    std::cout << "Bounds: X[" << minX << "," << maxX << "] "
          << "Y[" << minY << "," << maxY << "] "
          << "Z[" << minZ << "," << maxZ << "] "
          << "Radius: " << radius << "\n";
    std::cout << "Center: (" << centerX << ", " << centerY << ", " << centerZ << ")\n";
}

/**
 * @brief Opens a file dialog and loads a neuron morphology file
 * @param[out] nodes Vector to store the loaded SWC nodes
 * @param[out] minX Minimum X coordinate of the bounding box
 * @param[out] maxX Maximum X coordinate of the bounding box
 * @param[out] minY Minimum Y coordinate of the bounding box
 * @param[out] maxY Maximum Y coordinate of the bounding box
 * @param[out] minZ Minimum Z coordinate of the bounding box
 * @param[out] maxZ Maximum Z coordinate of the bounding box
 * @param[out] centerX X coordinate of the bounding box center
 * @param[out] centerY Y coordinate of the bounding box center
 * @param[out] centerZ Z coordinate of the bounding box center
 * @param[out] radius Radius of the bounding sphere
 * 
 * This function opens a file dialog for the user to select an SWC or UGX file,
 * loads the selected file, and computes the spatial bounds of the neuron.
 * The function updates both the provided output parameters and the global state.
 *
 * The function performs the following steps:
 * 1. Shows a file dialog for selecting SWC or UGX files
 * 2. Loads the selected file using loadSWC()
 * 3. Updates the global currentNodes and currentLoadedFile variables
 * 4. Computes the spatial bounds and center of the loaded neuron
 * 5. Resets the refinement delta to its default value
 *
 * @note Uses tinyfiledialogs for the file dialog
 * @note Prints diagnostic information to the console
 * @see loadSWC() for the actual file loading implementation
 * @see computeBounds() for the bounds calculation
 */
void openNeuronFile(std::vector<SWCNode>& nodes,
                    double& minX, double& maxX,
                    double& minY, double& maxY,
                    double& minZ, double& maxZ,
                    double& centerX, double& centerY,
                    double& centerZ, double& radius)
{
    const char* filters[] = { "*.swc", "*.ugx" };
    const char* file = tinyfd_openFileDialog("Open 1D Neuron File", "", 2, filters, "SWC or UGX Files", 0);
    if (!file) {
        std::cout << "[Info] No file selected.\n";
        return;
    }

    std::string filename(file);
    currentLoadedFile = filename;

    // Load and assign directly
    nodes = loadSWC(filename);
    currentNodes = nodes; 

    std::cout << "[Loaded] " << filename << " with " << nodes.size() << " nodes.\n";

    if (!nodes.empty()) {
        std::cout << "  First node: id=" << nodes[0].id << ", "
                  << "x=" << nodes[0].x << ", y=" << nodes[0].y
                  << ", z=" << nodes[0].z << ", r=" << nodes[0].radius << "\n";
    }

    // Compute bounds for camera and rendering
    computeBounds(nodes, minX, maxX, minY, maxY, minZ, maxZ, centerX, centerY, centerZ, radius);

    std::cout << "[Bounds] X: " << minX << "–" << maxX
              << " Y: " << minY << "–" << maxY
              << " Z: " << minZ << "–" << maxZ
              << " Center: (" << centerX << ", " << centerY << ", " << centerZ << ")"
              << " Radius: " << radius << "\n";

    // Reset refinement step
    refineDelta = 8.0;
    std::cout << "[Reset] refineDelta = 8.0\n";
}