#include "neurongraph.h"
#include <numeric>
#include <tinyxml2.h>

using namespace tinyxml2;

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

void NeuronGraph::readFromFileUGX(const std::string& filename)
{
    nodes.clear();
    edges.clear();

    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load UGX file: " << filename << std::endl;
        return;
    }

    XMLElement* root = doc.FirstChildElement("grid");
    if (!root) {
        std::cerr << "Invalid UGX file: no <grid> found.\n";
        return;
    }

    // --- 1. Extract coordinates ---
    XMLElement* vertsElem = root->FirstChildElement("vertices");
    std::vector<std::array<double, 3>> positions;

    if (vertsElem) {
        const char* coordsText = vertsElem->GetText();
        std::istringstream iss(coordsText);
        double x, y, z;
        while (iss >> x >> y >> z)
            positions.push_back({x, y, z});
    }

    int numVertices = static_cast<int>(positions.size());

    // --- 2. Extract diameters ---
    std::vector<double> diameters(numVertices, 1.0);  // default radius
    for (XMLElement* va = root->FirstChildElement("vertex_attachment"); va; va = va->NextSiblingElement("vertex_attachment")) {
        const char* name = va->Attribute("name");
        if (name && std::string(name) == "diameter") {
            const char* dText = va->GetText();
            if (dText) {
                std::istringstream dStream(dText);
                for (int i = 0; i < numVertices && dStream; ++i)
                    dStream >> diameters[i];
            }
            break;
        }
    }

    // --- 3. Extract edges (i j pairs) ---
    std::vector<std::pair<int, int>> edgeList;
    XMLElement* edgesElem = root->FirstChildElement("edges");
    if (edgesElem && edgesElem->GetText()) {
        std::istringstream es(edgesElem->GetText());
        int from, to;
        while (es >> from >> to)
            edgeList.emplace_back(from, to);
    }

    // --- 4. Extract subset types ---
    std::vector<int> types(numVertices, 0);  // default to 0 (undefined)

    XMLElement* subsetHandler = root->FirstChildElement("subset_handler");
    if (subsetHandler) {
        for (XMLElement* subset = subsetHandler->FirstChildElement("subset"); subset; subset = subset->NextSiblingElement("subset")) {
            const char* name = subset->Attribute("name");
            int typeCode = 0;
            if (name) {
                std::string sname(name);
                if (sname == "soma") typeCode = 1;
                else if (sname == "axon") typeCode = 2;
                else if (sname == "dend") typeCode = 3;
                else if (sname == "apic") typeCode = 4;
                else if (sname == "fork") typeCode = 5;
                else if (sname == "end")  typeCode = 6;
                else                      typeCode = 7;
            }

            XMLElement* vElem = subset->FirstChildElement("vertices");
            if (vElem && vElem->GetText()) {
                std::istringstream vs(vElem->GetText());
                int vi;
                while (vs >> vi) {
                    if (vi >= 0 && vi < numVertices)
                        types[vi] = typeCode;
                }
            }
        }
    }

    // --- 5. Build SWC nodes directly into `nodes` ---
    for (int i = 0; i < numVertices; ++i) {
        SWCNode node;
        node.id     = i + 1;
        node.type   = types[i];
        node.x      = positions[i][0];
        node.y      = positions[i][1];
        node.z      = positions[i][2];
        node.radius = diameters[i];
        node.pid    = -1;  // will be set via edgeList below
        nodes[node.id] = node;
    }

    // --- 6. Populate `edges` and update parent ids ---
    for (const auto& [from, to] : edgeList) {
        int parentId = from + 1;
        int childId  = to + 1;

        nodes[childId].pid = parentId;
        edges[parentId].push_back(childId);
    }

    std::cout << "Read UGX ..." << filename << std::endl;
}
