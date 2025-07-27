/**
 * @file neuronugx.cpp
 * @brief Implementation of UGX file format support for neuron morphologies
 * 
 * This file provides functionality to read from and write to the UGX file format,
 * which is commonly used in the UG4 simulation framework. It handles the conversion
 * between the internal SWC-based neuron representation and the UG4 grid format.
 *
 * Key features:
 * - Export neuron morphologies to UGX format for visualization and simulation
 * - Import UGX files while preserving node properties and connectivity
 * - Support for different neuron components (soma, axon, dendrites) via subsets
 * - Maintains 3D coordinates, diameters, and topological relationships
 *
 * The UGX format is an XML-based format that represents graphs with additional
 * attributes, making it suitable for complex neuron morphologies with different
 * types of neural processes.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 * @see https://github.com/yourusername/refactoredCPPNeuronMesher
 */

#include <array>
#include "neurongraph.h"
#include <numeric>
#include <tinyxml2.h>

using namespace tinyxml2;

/**
 * @brief Writes neuron morphology data to a UGX file format
 * @param nodeSet Map of SWC nodes to be written, where key is node ID and value is SWCNode object
 * @param filename Path to the output UGX file
 * 
 * This method converts the internal neuron representation into the UGX file format,
 * which is compatible with the UG4 simulation framework. The UGX format is an XML-based
 * format that can represent complex graph structures with additional attributes.
 *
 * The output UGX file includes:
 * - 3D vertex coordinates for each node
 * - Edge connectivity based on parent-child relationships
 * - Node diameters as vertex attributes
 * - Subset definitions for different neuron components (soma, axon, dendrites, etc.)
 *
 * The method performs the following steps:
 * 1. Extracts node positions, diameters, and connectivity from the input node set
 * 2. Creates a mapping between node IDs and internal indices
 * 3. Organizes nodes into subsets based on their type (soma, axon, dendrites, etc.)
 * 4. Constructs the UGX XML structure with proper formatting
 * 5. Writes the XML data to the specified file
 *
 * @note The method preserves the 3D structure and topological relationships of the neuron
 * @note If the file cannot be written, an error message is printed to stderr
 * @note The method automatically handles different SWC node types (soma=1, axon=2, etc.)
 * @see readFromFileUGX() for the corresponding import functionality
 * @see https://github.com/UG4/ug4 for more information about the UGX format
 */
void NeuronGraph::writeToFileUGX(const std::map<int, SWCNode>& nodeSet,
                               const std::string& filename) {
    std::vector<std::array<double, 3>> positions;
    std::vector<double> diameters;
    std::vector<std::pair<int, int>> edges;
    std::map<int, std::vector<int>> subsets; // type → list of node indices

    std::map<int, int> idToIndex;
    std::vector<int> indexToId;

    int index = 0;
    for (const auto& [id, node] : nodeSet) {
        idToIndex[id] = index++;
        indexToId.push_back(id);

        positions.push_back({node.x, node.y, node.z});
        diameters.push_back(node.radius);
        subsets[node.type].push_back(id);

        if (node.pid != -1 && nodeSet.count(node.pid)) {
            edges.emplace_back(node.pid, node.id);
        }
    }

    XMLDocument doc;
    doc.InsertFirstChild(doc.NewDeclaration(R"(xml version="1.0" encoding="utf-8")"));

    XMLElement* root = doc.NewElement("grid");
    root->SetAttribute("name", "defGrid");
    doc.InsertEndChild(root);

    // <vertices coords="3">x y z ...</vertices>
    XMLElement* vertsElem = doc.NewElement("vertices");
    vertsElem->SetAttribute("coords", "3");
    std::ostringstream coordStream;
    for (const auto& pos : positions)
        coordStream << pos[0] << " " << pos[1] << " " << pos[2] << " ";
    std::string coordStr = coordStream.str();
    if (!coordStr.empty()) coordStr.pop_back();
    vertsElem->SetText(coordStr.c_str());
    root->InsertEndChild(vertsElem);

    // <edges vertices="i j i j ..."/>
    XMLElement* edgesElem = doc.NewElement("edges");
    std::ostringstream edgeStream;
    for (const auto& [pid, id] : edges)
        edgeStream << idToIndex[pid] << " " << idToIndex[id] << " ";
    std::string edgeStr = edgeStream.str();
    if (!edgeStr.empty()) edgeStr.pop_back();
    edgesElem->SetText(edgeStr.c_str());
    root->InsertEndChild(edgesElem);

    // <vertex_attachment name="diameter">
    XMLElement* diamElem = doc.NewElement("vertex_attachment");
    diamElem->SetAttribute("name", "diameter");
    diamElem->SetAttribute("type", "double");
    diamElem->SetAttribute("passOn", "0");
    diamElem->SetAttribute("global", "1");
    std::ostringstream dstream;
    for (double r : diameters)
        dstream << r << " ";
    std::string dstr = dstream.str();
    if (!dstr.empty()) dstr.pop_back();
    diamElem->SetText(dstr.c_str());
    root->InsertEndChild(diamElem);

    // <subset_handler>
    XMLElement* shElem = doc.NewElement("subset_handler");
    shElem->SetAttribute("name", "defSH");

    if (subsets.empty()) {
        XMLElement* subset = doc.NewElement("subset");
        subset->SetAttribute("name", "neurite");
        subset->SetAttribute("color", "0.5 0.5 0.5");
        subset->SetAttribute("state", "0");

        std::ostringstream vi, ei;
        for (size_t i = 0; i < positions.size(); ++i) vi << i << " ";
        for (size_t i = 0; i < edges.size(); ++i) ei << i << " ";

        std::string vis = vi.str(), eis = ei.str();
        if (!vis.empty()) vis.pop_back();
        if (!eis.empty()) eis.pop_back();

        XMLElement* v = doc.NewElement("vertices");
        v->SetText(vis.c_str());
        subset->InsertEndChild(v);

        XMLElement* e = doc.NewElement("edges");
        e->SetText(eis.c_str());
        subset->InsertEndChild(e);

        shElem->InsertEndChild(subset);
    } else {
        for (const auto& [type, ids] : subsets) {
            std::string name = "neurite";
            if (type == 1) name = "soma";
            else if (type == 2) name = "axon";
            else if (type == 3) name = "dend";
            else if (type == 4) name = "apic";
            else if (type == 5) name = "fork";
            else if (type == 6) name = "end";

            XMLElement* subset = doc.NewElement("subset");
            subset->SetAttribute("name", name.c_str());
            subset->SetAttribute("color", "0.7 0.7 0.2");
            subset->SetAttribute("state", "0");

            std::ostringstream vi, ei;
            for (size_t i = 0; i < ids.size(); ++i) {
                vi << idToIndex[ids[i]] << " ";
            }

            for (size_t i = 0; i < edges.size(); ++i) {
                if (nodeSet.at(edges[i].second).type == type)
                    ei << i << " ";
            }

            std::string vis = vi.str(), eis = ei.str();
            if (!vis.empty()) vis.pop_back();
            if (!eis.empty()) eis.pop_back();

            XMLElement* v = doc.NewElement("vertices");
            v->SetText(vis.c_str());
            subset->InsertEndChild(v);

            XMLElement* e = doc.NewElement("edges");
            e->SetText(eis.c_str());
            subset->InsertEndChild(e);

            shElem->InsertEndChild(subset);
        }
    }

    root->InsertEndChild(shElem);

    // <projection_handler>
    XMLElement* projElem = doc.NewElement("projection_handler");
    projElem->SetAttribute("name", "defPH");
    projElem->SetAttribute("subset_handler", "0");

    XMLElement* def = doc.NewElement("default");
    def->SetAttribute("type", "default");
    def->SetText("0 0");

    projElem->InsertEndChild(def);
    root->InsertEndChild(projElem);

    // Write file
    if (doc.SaveFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to write UGX file: " << filename << std::endl;
    } else {
        std::cout << "WriteUGX ..." << filename << std::endl;
    }
}

/**
 * @brief Reads neuron morphology data from a UGX file
 * @param filename Path to the input UGX file to read
 * 
 * This method imports neuron morphology data from a UGX file and populates the internal
 * data structures of the NeuronGraph class. It handles the conversion from the UGX format
 * to the internal SWC-based representation.
 *
 * The method processes the following UGX elements:
 * - Vertex coordinates (3D positions)
 * - Edge connectivity between nodes
 * - Vertex attributes (diameters, types, etc.)
 * - Subset information for different neuron components
 *
 * The import process includes:
 * 1. Parsing the XML structure of the UGX file
 * 2. Extracting vertex coordinates and creating SWC nodes
 * 3. Reconstructing parent-child relationships from edges
 * 4. Loading additional attributes like diameters and node types
 * 5. Validating the imported morphology
 *
 * @note The method clears any existing neuron data before importing
 * @note If the file cannot be read or is malformed, error messages are printed to stderr
 * @note The method attempts to map UGX subsets back to SWC node types when possible
 * @see writeToFileUGX() for the corresponding export functionality
 * @see https://github.com/UG4/ug4 for more information about the UGX format
 */
void NeuronGraph::readFromFileUGX(const std::string& filename)
{
    nodes.clear();
    edges.clear();

    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "[UGX Error] Failed to load: " << filename << std::endl;
        return;
    }

    XMLElement* root = doc.FirstChildElement("grid");
    if (!root) {
        std::cerr << "[UGX Error] Missing <grid> root element.\n";
        return;
    }

    // --- 1. Extract coordinates ---
    std::vector<std::array<double, 3>> positions;
    XMLElement* vertsElem = root->FirstChildElement("vertices");
    if (!vertsElem || !vertsElem->GetText()) {
        std::cerr << "[UGX Warning] No vertex data found.\n";
        return;
    }

    std::istringstream coordStream(vertsElem->GetText());
    double x, y, z;
    while (coordStream >> x >> y >> z) {
        positions.push_back({x, y, z});
    }
    int numVertices = static_cast<int>(positions.size());
    std::cout << "[UGX] Parsed " << numVertices << " vertices.\n";

    // --- 2. Extract diameters ---
    std::vector<double> diameters(numVertices, 1.0);  // fallback default
    for (XMLElement* va = root->FirstChildElement("vertex_attachment"); va; va = va->NextSiblingElement("vertex_attachment")) {
        const char* name = va->Attribute("name");
        if (name && std::string(name) == "diameter") {
            const char* dText = va->GetText();
            if (dText) {
                std::istringstream dStream(dText);
                for (int i = 0; i < numVertices && dStream; ++i)
                    dStream >> diameters[i];
                std::cout << "[UGX] Parsed diameter values.\n";
            }
            break;
        }
    }

    // --- 3. Extract edges ---
    std::vector<std::pair<int, int>> edgeList;
    XMLElement* edgesElem = root->FirstChildElement("edges");
    if (edgesElem && edgesElem->GetText()) {
        std::istringstream edgeStream(edgesElem->GetText());
        int from, to;
        while (edgeStream >> from >> to) {
            edgeList.emplace_back(from, to);
        }
        std::cout << "[UGX] Parsed " << edgeList.size() << " edges.\n";
    } else {
        std::cout << "[UGX] No edge list found.\n";
    }

    // --- 4. Extract subset types ---
    std::vector<int> types(numVertices, 0);  // default type 0 (undefined)
    XMLElement* subsetHandler = root->FirstChildElement("subset_handler");
    if (subsetHandler) {
        for (XMLElement* subset = subsetHandler->FirstChildElement("subset"); subset; subset = subset->NextSiblingElement("subset")) {
            const char* name = subset->Attribute("name");
            int typeCode = 0;
            if (name) {
                std::string sname(name);
                if      (sname == "soma")  typeCode = 1;
                else if (sname == "axon")  typeCode = 2;
                else if (sname == "dend")  typeCode = 3;
                else if (sname == "apic")  typeCode = 4;
                else if (sname == "fork")  typeCode = 5;
                else if (sname == "end")   typeCode = 6;
                else                       typeCode = 7; // unknown but labeled
            }

            XMLElement* vElem = subset->FirstChildElement("vertices");
            if (vElem && vElem->GetText()) {
                std::istringstream vs(vElem->GetText());
                int vi;
                while (vs >> vi) {
                    if (vi >= 0 && vi < numVertices)
                        types[vi] = typeCode;
                    else
                        std::cerr << "[UGX Warning] Invalid vertex index in subset: " << vi << "\n";
                }
            }
        }
    }

    // --- 5. Create SWCNodes ---
    for (int i = 0; i < numVertices; ++i) {
        SWCNode node;
        node.id     = i + 1;
        node.type   = types[i];
        node.x      = positions[i][0];
        node.y      = positions[i][1];
        node.z      = positions[i][2];
        node.radius = diameters[i];
        node.pid    = -1;
        nodes[node.id] = node;
    }

    // --- 6. Build edges + set parents ---
    for (const auto& [from, to] : edgeList) {
        int parentId = from + 1;
        int childId  = to + 1;

        if (nodes.count(childId) && nodes.count(parentId)) {
            nodes[childId].pid = parentId;
            edges[parentId].push_back(childId);
        } else {
            std::cerr << "[UGX Warning] Invalid edge (" << from << " → " << to << ")\n";
        }
    }

    std::cout << "Read UGX ... " << filename << " with "
              << nodes.size() << " nodes and "
              << edges.size() << " parent entries.\n";
}