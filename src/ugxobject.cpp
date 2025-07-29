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

	// --- 4. Load Subset Info ---
	XMLElement* subsetHandler = root->FirstChildElement("subset_handler");
	if (subsetHandler) {
		int subsetIndex = 0;
		for (XMLElement* subset = subsetHandler->FirstChildElement("subset");
			subset; subset = subset->NextSiblingElement("subset"), ++subsetIndex) {

			const char* nameAttr = subset->Attribute("name");
			std::string subsetName = nameAttr ? nameAttr : "unnamed";
			ugxg.subsetNames[subsetIndex] = subsetName;

			// Vertex membership
			if (XMLElement* vElem = subset->FirstChildElement("vertices"); vElem && vElem->GetText()) {
				std::istringstream vStream(vElem->GetText());
				int vid;
				while (vStream >> vid)
					ugxg.vertexSubsets[vid] = subsetIndex;
			}

			// Edge membership
			if (XMLElement* eElem = subset->FirstChildElement("edges"); eElem && eElem->GetText()) {
				std::istringstream eStream(eElem->GetText());
				int eid;
				while (eStream >> eid)
					ugxg.edgeSubsets[eid] = subsetIndex;
			}

			// Face membership
			if (XMLElement* fElem = subset->FirstChildElement("faces"); fElem && fElem->GetText()) {
				std::istringstream fStream(fElem->GetText());
				int fid;
				while (fStream >> fid)
					ugxg.faceSubsets[fid] = subsetIndex;
			}
		}

		std::cout << "[UGXObject] Loaded " << ugxg.subsetNames.size()
				<< " subsets from " << filename << std::endl;
	}
}

void UgxObject::printCoordinates() const {
    if (ugxg.points.empty()) {
        std::cout << "No points to display.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "ID\tX\tY\tZ\tSubset\n";
	for (const auto& [id, coord] : ugxg.points) {
		int subsetId = ugxg.vertexSubsets.count(id) ? ugxg.vertexSubsets.at(id) : -1;
		std::string subsetName = subsetId >= 0 && ugxg.subsetNames.count(subsetId)
								? ugxg.subsetNames.at(subsetId) : "n/a";
		std::cout << id << "\t" << coord.x << "\t" << coord.y << "\t"
				<< coord.z << "\t" << subsetName << "\n";
	}
}

void UgxObject::printEdges() const {
    if (ugxg.edges.empty()) {
        std::cout << "No edges to display.\n";
        return;
    }

    std::cout << "Edge (v0 → v1)\tSubset\n";
    for (size_t i = 0; i < ugxg.edges.size(); ++i) {
        auto [v0, v1] = ugxg.edges[i];
        int subsetId = ugxg.edgeSubsets.count(i) ? ugxg.edgeSubsets.at(i) : -1;
        std::string name = (subsetId >= 0 && ugxg.subsetNames.count(subsetId))
                           ? ugxg.subsetNames.at(subsetId) : "n/a";
        std::cout << v0 << " → " << v1 << "\t" << name << "\n";
    }
}

void UgxObject::printFaces() const {
    if (ugxg.faces.empty()) {
        std::cout << "No faces to display.\n";
        return;
    }

    std::cout << "Face (v0, v1, v2)\tSubset\n";
    for (size_t i = 0; i < ugxg.faces.size(); ++i) {
        const auto& face = ugxg.faces[i];
        int subsetId = ugxg.faceSubsets.count(i) ? ugxg.faceSubsets.at(i) : -1;
        std::string name = (subsetId >= 0 && ugxg.subsetNames.count(subsetId))
                           ? ugxg.subsetNames.at(subsetId) : "n/a";
        std::cout << face[0] << ", " << face[1] << ", " << face[2]
                  << "\t" << name << "\n";
    }
}