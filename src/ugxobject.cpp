#include "ugxobject.h"
#include <tinyxml2.h>
#include <iomanip> // for std::setprecision

using namespace tinyxml2;

void UgxObject::readUGX(const std::string& filename) {
    ugxg.points.clear();
    ugxg.edges.clear();
    ugxg.faces.clear();

    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "[UGXObject Error] Failed to load: " << filename << std::endl;
        return;
    }

    XMLElement* root = doc.FirstChildElement("grid");
    if (!root) {
        std::cerr << "[UGXObject Error] Missing <grid> root element in: " << filename << std::endl;
        return;
    }

    // --- 1. Load Points ---
    XMLElement* vertsElem = root->FirstChildElement("vertices");
    if (vertsElem && vertsElem->GetText()) {
        std::istringstream coordStream(vertsElem->GetText());
        double x, y, z;
        int index = 0;

        while (coordStream >> x >> y >> z) {
            ugxg.points[index++] = {x, y, z};
        }
        std::cout << "[UGXObject] Loaded " << ugxg.points.size() << " points from " << filename << std::endl;
    } else {
        std::cerr << "[UGXObject Warning] No vertex data found in: " << filename << std::endl;
    }

    // --- 2. Load Edges (if present) ---
    XMLElement* edgesElem = root->FirstChildElement("edges");
    if (edgesElem && edgesElem->GetText()) {
        std::istringstream edgeStream(edgesElem->GetText());
        int from, to;
        while (edgeStream >> from >> to) {
            ugxg.edges.emplace_back(from, to);
        }
        std::cout << "[UGXObject] Loaded " << ugxg.edges.size() << " edges from " << filename << std::endl;
    }

    // --- 3. Load Faces (optional) ---
    XMLElement* facesElem = root->FirstChildElement("triangles");
    if (facesElem && facesElem->GetText()) {
        std::istringstream faceStream(facesElem->GetText());
        int v0, v1, v2;
        while (faceStream >> v0 >> v1 >> v2) {
            ugxg.faces.push_back({v0, v1, v2});
        }
        std::cout << "[UGXObject] Loaded " << ugxg.faces.size() << " faces from " << filename << std::endl;
    }
}

void UgxObject::printCoordinates() const {
    if (ugxg.points.empty()) {
        std::cout << "No points to display.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "ID\tX\tY\tZ\n";
    for (const auto& [id, coord] : ugxg.points) {
        std::cout << id << "\t" << coord.x << "\t" << coord.y << "\t" << coord.z << "\n";
    }
}

void UgxObject::printEdges() const {
    if (ugxg.edges.empty()) {
        std::cout << "No edges to display.\n";
        return;
    }

    std::cout << "Edges (vertexID0 → vertexID1):\n";
    for (const auto& [from, to] : ugxg.edges) {
        std::cout << from << " → " << to << "\n";
    }
}

void UgxObject::printFaces() const {
    if (ugxg.faces.empty()) {
        std::cout << "No faces to display.\n";
        return;
    }

    std::cout << "Faces (triangle vertices):\n";
    for (const auto& face : ugxg.faces) {
        std::cout << face[0] << ", " << face[1] << ", " << face[2] << "\n";
    }
}
