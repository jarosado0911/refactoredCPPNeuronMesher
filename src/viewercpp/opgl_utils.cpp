// opengl tools

#include "opgl_utils.h"

std::string currentLoadedFile = "";

void printHelpText() {
    std::cout << "\n=== SWC Viewer Controls ===\n";
    std::cout << "Key/Mouse     | Function\n";
    std::cout << "1 - 6         | Change render mode\n";
    std::cout << "O             | Open SWC file\n";
    std::cout << "R             | Reset camera view\n";
    std::cout << "Mouse Drag    | Rotate scene (Left Button)\n";
    std::cout << "Mouse Drag    | Pan scene (Right Button)\n";
    std::cout << "Scroll Wheel  | Zoom in/out\n";
    std::cout << "ESC           | Exit viewer\n\n";
}

std::vector<SWCNode> loadSWC(const std::string& filename) {
    std::vector<SWCNode> nodes;
    NeuronGraph graph;
    graph.readFromFileUGXorSWC(filename);

    for(auto& [_,node] : graph.getNodes())
    {
        nodes.push_back(node);
    }

    return nodes;
}

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
}

void openNeuronFile(std::vector<SWCNode>& nodes, double& minX, double& maxX, double& minY, double& maxY, double& minZ, double& maxZ, double& centerX, double& centerY, double& centerZ, double& radius) {
    const char* filters[] = { "*.swc" };
    const char* file = tinyfd_openFileDialog("Open SWC File", "", 1, filters, "SWC files", 0);

    if (file) {
        std::cout << "Loading: " << file << "\n";
        currentLoadedFile = file; 
        nodes = loadSWC(file);
        computeBounds(nodes, minX, maxX, minY, maxY, minZ, maxZ, centerX, centerY, centerZ, radius);
    }
}
