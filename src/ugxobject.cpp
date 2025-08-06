#include "neurongraph.h"
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

    // Parse radius or diameter from vertex_attachment blocks (as flat value list)
    for (tinyxml2::XMLElement* attach = root->FirstChildElement("vertex_attachment");
        attach; attach = attach->NextSiblingElement("vertex_attachment")) {

        const char* name = attach->Attribute("name");
        if (!name) continue;

        bool isDiameter = std::string(name) == "diameter";
        bool isRadius   = std::string(name) == "radius";
        if (!isDiameter && !isRadius) continue;

        const char* rawText = attach->GetText();
        if (!rawText) continue;

        std::istringstream stream(rawText);
        double value;
        int index = 0;

        while (stream >> value) {
            ugxg.radii[index++] = isDiameter ? value / 2.0 : value;
        }

        std::cout << "[UGXObject] Parsed " << index << " values for " << name << std::endl;
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

void UgxObject::writeUGX(const std::string& filename) const {
    XMLDocument doc;
    doc.InsertFirstChild(doc.NewDeclaration());

    XMLElement* root = doc.NewElement("grid");
    root->SetAttribute("name", "defGrid");
    doc.InsertEndChild(root);

    // --- Write Vertices ---
    XMLElement* vertsElem = doc.NewElement("vertices");
    vertsElem->SetAttribute("coords", "3");

    std::ostringstream coordStream;
    for (const auto& [id, coord] : ugxg.points)
        coordStream << coord.x << " " << coord.y << " " << coord.z << " ";

    std::string coordStr = coordStream.str();
    if (!coordStr.empty()) coordStr.pop_back();  // remove trailing space
    vertsElem->SetText(coordStr.c_str());
    root->InsertEndChild(vertsElem);

    // --- Write Edges ---
    if (!ugxg.edges.empty()) {
        XMLElement* edgesElem = doc.NewElement("edges");
        std::ostringstream edgeStream;
        for (const auto& [v0, v1] : ugxg.edges)
            edgeStream << v0 << " " << v1 << " ";
        std::string edgeStr = edgeStream.str();
        if (!edgeStr.empty()) edgeStr.pop_back();
        edgesElem->SetText(edgeStr.c_str());
        root->InsertEndChild(edgesElem);
    }

    // --- Write Faces ---
    if (!ugxg.faces.empty()) {
        XMLElement* faceElem = doc.NewElement("triangles");
        std::ostringstream faceStream;
        for (const auto& face : ugxg.faces)
            faceStream << face[0] << " " << face[1] << " " << face[2] << " ";
        std::string faceStr = faceStream.str();
        if (!faceStr.empty()) faceStr.pop_back();
        faceElem->SetText(faceStr.c_str());
        root->InsertEndChild(faceElem);
    }


    // --- Write Radii as Vertex Attachment ---
    if (!ugxg.radii.empty()) {
        XMLElement* attachElem = doc.NewElement("vertex_attachment");
        attachElem->SetAttribute("name", "diameter");
        attachElem->SetAttribute("type", "double");
        attachElem->SetAttribute("passOn", "0");
        attachElem->SetAttribute("global", "1");

        // Create ordered output string by vertex index
        std::ostringstream oss;
        for (size_t i = 0; i < ugxg.points.size(); ++i) {
            double radius = 0.0;
            auto it = ugxg.radii.find(i);
            if (it != ugxg.radii.end())
                oss << (2.0 * it->second) << " ";  // write as diameter
            else
                oss << "0.0 ";  // fallback if missing
        }

        std::string radiiText = oss.str();
        if (!radiiText.empty()) radiiText.pop_back();  // remove trailing space

        attachElem->SetText(radiiText.c_str());
        root->InsertEndChild(attachElem);
    }

    // --- Write Subsets ---
    if (!ugxg.subsetNames.empty()) {
        XMLElement* shElem = doc.NewElement("subset_handler");
        shElem->SetAttribute("name", "defSH");

        for (const auto& [subsetId, subsetName] : ugxg.subsetNames) {
            XMLElement* subset = doc.NewElement("subset");
            subset->SetAttribute("name", subsetName.c_str());
            subset->SetAttribute("state", "0");
            subset->SetAttribute("color", "0.5 0.5 0.5");

            // Vertices
            std::ostringstream vi;
            for (const auto& [vid, sid] : ugxg.vertexSubsets)
                if (sid == subsetId) vi << vid << " ";
            std::string vis = vi.str();
            if (!vis.empty()) vis.pop_back();

            if (!vis.empty()) {
                XMLElement* verts = doc.NewElement("vertices");
                verts->SetText(vis.c_str());
                subset->InsertEndChild(verts);
            }

            // Edges
            std::ostringstream ei;
            for (const auto& [eid, sid] : ugxg.edgeSubsets)
                if (sid == subsetId) ei << eid << " ";
            std::string eis = ei.str();
            if (!eis.empty()) eis.pop_back();

            if (!eis.empty()) {
                XMLElement* edges = doc.NewElement("edges");
                edges->SetText(eis.c_str());
                subset->InsertEndChild(edges);
            }

            // Faces
            std::ostringstream fi;
            for (const auto& [fid, sid] : ugxg.faceSubsets)
                if (sid == subsetId) fi << fid << " ";
            std::string fis = fi.str();
            if (!fis.empty()) fis.pop_back();

            if (!fis.empty()) {
                XMLElement* faces = doc.NewElement("faces");
                faces->SetText(fis.c_str());
                subset->InsertEndChild(faces);
            }

            shElem->InsertEndChild(subset);
        }

        root->InsertEndChild(shElem);
    }

    // --- Save File ---
    if (doc.SaveFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "[UGXObject Error] Failed to write: " << filename << std::endl;
    } else {
        std::cout << "[UGXObject] Successfully wrote: " << filename << std::endl;
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

const UgxGeometry UgxObject::convertToUGX(const std::map<int, SWCNode>& nodeSet) {
    UgxGeometry ugxout;

    std::map<int, int> typeToSubsetIndex;           // SWC type → subset index
    std::map<int, std::string> typeToSubsetName;    // SWC type → name
    std::map<int, int> swcToUgxIndex;               // SWC id → UGX vertex index

    int nextIndex = 0;
    int nextSubsetIndex = 0;

    // Step 1: Define subsets and points
    for (const auto& [id, node] : nodeSet) {
        int type = node.type;

        // Create subset for new type
        if (typeToSubsetIndex.count(type) == 0) {
            typeToSubsetIndex[type] = nextSubsetIndex;
            typeToSubsetName[type] = "type_" + std::to_string(type);
            ugxout.subsetNames[nextSubsetIndex] = typeToSubsetName[type];
            ++nextSubsetIndex;
        }

        int ugxIndex = nextIndex++;
        swcToUgxIndex[id] = ugxIndex;
        ugxout.points[ugxIndex] = {node.x, node.y, node.z};
        ugxout.radii[ugxIndex] = node.radius;  // <-- NEW
        ugxout.vertexSubsets[ugxIndex] = typeToSubsetIndex[type];
    }

    // Step 2: Add edges based on parent-child relationship
    for (const auto& [id, node] : nodeSet) {
        if (node.pid == -1) continue;

        auto itFrom = swcToUgxIndex.find(node.pid);
        auto itTo = swcToUgxIndex.find(id);

        if (itFrom != swcToUgxIndex.end() && itTo != swcToUgxIndex.end()) {
            int from = itFrom->second;
            int to = itTo->second;

            ugxout.edges.emplace_back(from, to);

            // Assign edge to same subset as child node
            int subset = typeToSubsetIndex[node.type];
            ugxout.edgeSubsets[ugxout.edges.size() - 1] = subset;
        }
    }

    return ugxout;
}

UgxGeometry UgxObject::addUGXGeometry(const UgxGeometry& geometry1, const UgxGeometry& geometry2) {
    UgxGeometry combined;

    // Step 1: Copy and insert all points from geometry1
    combined.points = geometry1.points;
    combined.radii = geometry1.radii;
    combined.vertexSubsets = geometry1.vertexSubsets;
    combined.edges = geometry1.edges;
    combined.edgeSubsets = geometry1.edgeSubsets;
    combined.faces = geometry1.faces;
    combined.faceSubsets = geometry1.faceSubsets;
    combined.subsetNames = geometry1.subsetNames;  // Start with geometry1's subset names

    // Determine current max indices
    int vertexOffset = combined.points.empty() ? 0 : (combined.points.rbegin()->first + 1);
    int edgeOffset = static_cast<int>(combined.edges.size());
    int faceOffset = static_cast<int>(combined.faces.size());

    // Step 2: Insert points from geometry2 with reindexed vertex IDs
    std::map<int, int> pointRemap;
    for (const auto& [id, coord] : geometry2.points) {
        int newId = id + vertexOffset;
        combined.points[newId] = coord;
        pointRemap[id] = newId;
    }

    // Step 3: Insert radii with new vertex IDs
    for (const auto& [id, r] : geometry2.radii) {
        int newId = pointRemap.at(id);
        combined.radii[newId] = r;
    }

    // Step 4: Insert vertex subsets
    for (const auto& [id, subset] : geometry2.vertexSubsets) {
        int newId = pointRemap.at(id);
        combined.vertexSubsets[newId] = subset;
    }

    // Step 5: Append edges (with reindexed vertices)
    for (size_t i = 0; i < geometry2.edges.size(); ++i) {
        auto [from, to] = geometry2.edges[i];
        combined.edges.emplace_back(pointRemap.at(from), pointRemap.at(to));

        int newEdgeIndex = edgeOffset + static_cast<int>(i);
        if (geometry2.edgeSubsets.count(static_cast<int>(i)))
            combined.edgeSubsets[newEdgeIndex] = geometry2.edgeSubsets.at(static_cast<int>(i));
    }

    // Step 6: Append faces (with reindexed vertices)
    for (size_t i = 0; i < geometry2.faces.size(); ++i) {
        auto [v0, v1, v2] = geometry2.faces[i];
        combined.faces.push_back({
            pointRemap.at(v0),
            pointRemap.at(v1),
            pointRemap.at(v2)
        });

        int newFaceIndex = faceOffset + static_cast<int>(i);
        if (geometry2.faceSubsets.count(static_cast<int>(i)))
            combined.faceSubsets[newFaceIndex] = geometry2.faceSubsets.at(static_cast<int>(i));
    }

    // Step 7: Merge subset names (if subset IDs collide, we assume the names are consistent)
    for (const auto& [subsetId, name] : geometry2.subsetNames) {
        // If already exists, keep geometry1's label
        if (combined.subsetNames.count(subsetId) == 0)
            combined.subsetNames[subsetId] = name;
    }

    return combined;
}
