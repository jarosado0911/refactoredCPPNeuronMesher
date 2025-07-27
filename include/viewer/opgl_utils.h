/**
 * @file opgl_utils.h
 * @brief OpenGL utility functions for neuron visualization
 * 
 * This header provides utility functions for OpenGL-based visualization of
 * neuron morphologies, including file I/O, bounds computation, and help text
 * display. These utilities handle the core functionality for loading and
 * preparing neuron data for rendering.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 */

#pragma once
#include <vector>  // For std::vector
#include <string>  // For std::string

// Forward declarations
struct SWCNode;

// External dependencies
#include "neurongraph.h"       // For NeuronGraph and SWCNode
#include "tinyfiledialogs.h"   // For file dialog functionality

/**
 * @brief Path to the currently loaded SWC file
 * 
 * Stores the full path to the most recently loaded SWC file.
 * Empty string if no file has been loaded yet.
 */
extern std::string currentLoadedFile;

/**
 * @brief Loads neuron data from an SWC file
 * 
 * Parses an SWC (Standardized Waveform Container) file containing
 * neuron morphology data and returns a vector of SWCNode objects.
 * 
 * @param filename Path to the SWC file to load
 * @return std::vector<SWCNode> Vector of nodes representing the neuron morphology
 * 
 * @throws std::runtime_error If the file cannot be opened or is malformed
 * 
 * @note The SWC format is a standard format for representing neuron morphologies.
 *       Each line in the file represents a point in the neuron structure.
 * @see http://www.neuronland.org/NLMorphologyConverter/MorphologyFormats/SWC/Spec.html
 */
std::vector<SWCNode> loadSWC(const std::string& filename);
/**
 * @brief Computes the spatial bounds and center of a set of nodes
 * 
 * Calculates the axis-aligned bounding box, center point, and maximum radius
 * of a collection of SWC nodes. This is used for camera positioning and scaling.
 * 
 * @param[in] nodes Vector of SWC nodes to analyze
 * @param[out] minX Minimum X coordinate of all nodes
 * @param[out] maxX Maximum X coordinate of all nodes
 * @param[out] minY Minimum Y coordinate of all nodes
 * @param[out] maxY Maximum Y coordinate of all nodes
 * @param[out] minZ Minimum Z coordinate of all nodes
 * @param[out] maxZ Maximum Z coordinate of all nodes
 * @param[out] centerX X coordinate of the center point
 * @param[out] centerY Y coordinate of the center point
 * @param[out] centerZ Z coordinate of the center point
 * @param[out] radius Maximum distance from center to any node
 * 
 * @note All output parameters are set to 0 if the input vector is empty
 */
void computeBounds(const std::vector<SWCNode>& nodes, double& minX, double& maxX, double& minY, double& maxY,
                   double& minZ, double& maxZ, double& centerX, double& centerY, double& centerZ, double& radius);
/**
 * @brief Opens a file dialog to load a neuron file and updates the view
 * 
 * Displays a file open dialog to the user and loads the selected SWC file.
 * Updates the provided node container and computes the new spatial bounds.
 * 
 * @param[out] nodes Vector to store the loaded nodes
 * @param[out] minX Minimum X coordinate (updated after loading)
 * @param[out] maxX Maximum X coordinate (updated after loading)
 * @param[out] minY Minimum Y coordinate (updated after loading)
 * @param[out] maxY Maximum Y coordinate (updated after loading)
 * @param[out] minZ Minimum Z coordinate (updated after loading)
 * @param[out] maxZ Maximum Z coordinate (updated after loading)
 * @param[out] centerX X coordinate of the center (updated after loading)
 * @param[out] centerY Y coordinate of the center (updated after loading)
 * @param[out] centerZ Z coordinate of the center (updated after loading)
 * @param[out] radius Maximum radius from center (updated after loading)
 * 
 * @return True if a file was successfully loaded, false otherwise
 * 
 * @note Uses tinyfiledialogs for cross-platform file dialog support
 * @note Updates currentLoadedFile on successful load
 */
void openNeuronFile(std::vector<SWCNode>& nodes, double& minX, double& maxX, double& minY, double& maxY,
                    double& minZ, double& maxZ, double& centerX, double& centerY, double& centerZ, double& radius);
/**
 * @brief Prints keyboard and mouse controls to the console
 * 
 * Displays a help message listing all available keyboard shortcuts
 * and mouse controls for the neuron viewer application.
 * 
 * @note This function is typically called when the application starts
 *       or when the user presses the 'H' key.
 */
void printHelpText();

