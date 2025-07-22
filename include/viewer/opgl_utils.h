// opengl tools
#pragma once
#include <vector>
#include "neurongraph.h"
#include "tinyfiledialogs.h"

extern std::string currentLoadedFile;

std::vector<SWCNode> loadSWC(const std::string& filename);
void computeBounds(const std::vector<SWCNode>& nodes, double& minX, double& maxX, double& minY, double& maxY,
                   double& minZ, double& maxZ, double& centerX, double& centerY, double& centerZ, double& radius);
void openNeuronFile(std::vector<SWCNode>& nodes, double& minX, double& maxX, double& minY, double& maxY,
                    double& minZ, double& maxZ, double& centerX, double& centerY, double& centerZ, double& radius);
void printHelpText();

