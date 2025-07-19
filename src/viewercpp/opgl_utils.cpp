// opengl tools

#include "opgl_utils.h"
#include "globals.h"

std::string currentLoadedFile = "";

void printHelpText() {
    std::cout << help;
}

std::vector<SWCNode> loadSWC(const std::string& filename) {
    std::vector<SWCNode> nodes;
    NeuronGraph graph;
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

void openNeuronFile(std::vector<SWCNode>& nodes,
                    double& minX, double& maxX,
                    double& minY, double& maxY,
                    double& minZ, double& maxZ,
                    double& centerX, double& centerY,
                    double& centerZ, double& radius)
{
    const char* file = tinyfd_openFileDialog("Open SWC", "", 0, nullptr, nullptr, 0);
    if (!file) {
        std::cout << "[Info] No file selected.\n";
        return;
    }

    std::string filename(file);
    currentLoadedFile = filename;

    // Load and assign directly
    nodes = loadSWC(filename);

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