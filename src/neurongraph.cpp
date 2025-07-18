#include "neurongraph.h"
#include "neuronugx.cpp"
#include "neuronoperations.cpp"
#include "neurontrunks.cpp"
#include <tinyxml2.h>

namespace fs = std::filesystem;

NeuronGraph::NeuronGraph(const std::map<int, SWCNode>& nodeSet){
	nodes.clear();
	edges.clear();

	for(auto& [_,node] : nodeSet){
		addNode(node);
	}
}

void NeuronGraph::setNodes(const std::map<int,SWCNode>& nodeSet){
	nodes.clear();
	edges.clear();

	for(auto& [_,node] : nodeSet){
		addNode(node);
	}
}

void NeuronGraph::addNode(const SWCNode& node){
	nodes[node.id] = node;
	if(node.pid != -1) {
	   edges[node.pid].push_back(node.id);
	}
}

std::map<int, SWCNode> NeuronGraph::preprocess(const std::map<int, SWCNode> & nodeSet) const {
			// check soma properties
			std::map<int, SWCNode> result = nodeSet;
			if (this->isSomaMissing(result)){
				result = this->setSoma(result);
			}
			if (this->hasSomaSegment(result)){
				result = this->removeSomaSegment(result);
			}
			return result;
}

void NeuronGraph::readFromFile(const std::string& filename) {
    nodes.clear();
    edges.clear();

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    int lineNum = 0;
    while (std::getline(infile, line)) {
        ++lineNum;

        // Skip empty or comment-only lines
        if (line.empty() || line[0] == '#') continue;

        // Strip inline comments
        auto commentPos = line.find('#');
        if (commentPos != std::string::npos)
            line = line.substr(0, commentPos);

        // Trim leading/trailing whitespace
        // Trim leading whitespace
		line.erase(0, line.find_first_not_of(" \t\r\n"));

		// Trim trailing whitespace only if line is not empty
		if (!line.empty()) {
			size_t end = line.find_last_not_of(" \t\r\n");
			if (end != std::string::npos)
				line.erase(end + 1);
		}

		// Replace tabs with spaces
		std::replace(line.begin(), line.end(), '\t', ' ');

        if (line.empty()) continue;  // was all comment or whitespace

        std::istringstream iss(line);
        SWCNode node;

        if (!(iss >> node.id >> node.type >> node.x >> node.y >> node.z >> node.radius >> node.pid)) {
            std::cerr << "[Parse Error] Line " << lineNum << ": '" << line << "'\n";
            continue;
        }

        addNode(node);
    }

    std::cout << "Read SWC ... " << filename << " with " << nodes.size() << " nodes.\n";
}


void NeuronGraph::writeToFile(const std::map<int, SWCNode> & nodeSet,
		              const std::string& filename){
	std::ofstream outfile(filename);
	if(!outfile.is_open()){
		std::cerr << "Failed to open output file: " << filename << std::endl;
		return;
	}

	std::vector<int> ids;
	for(const auto& [id,_] : nodeSet){
		ids.push_back(id);
	}
	std::sort(ids.begin(),ids.end());
	for(int id: ids) {
		const auto & node = nodeSet.at(id);
		outfile << node.id     << " "
			<< node.type   << " "
			<< node.x      << " "
			<< node.y      << " "
			<< node.z      << " "
			<< node.radius << " "
			<< node.pid    << "\n";
	}
	std::cout << "WriteSWC ..." << filename << std::endl;
}

void NeuronGraph::readFromFileUGXorSWC(const std::string& filename) {
    std::filesystem::path path(filename);
    std::string ext = path.extension().string();

    if (ext == ".swc") {
        readFromFile(filename);  // your SWC reader
    } else if (ext == ".ugx") {
        readFromFileUGX(filename);  // your UGX reader
    } else {
        std::cerr << "Unsupported file format: " << ext << "\n";
    }
}

void NeuronGraph::swc2ugx(const std::string& inputfile, const std::string& outputfile)
{
	NeuronGraph graph(inputfile);
	auto nodes = graph.getNodes();
	graph.writeToFileUGX(nodes,outputfile);
}

void NeuronGraph::ugx2swc(const std::string& inputfile, const std::string& outputfile)
{
	NeuronGraph graph(inputfile);
	auto nodes = graph.getNodes();
	graph.writeToFile(nodes,outputfile);
}

bool NeuronGraph::isTopologicallySorted(const std::map<int, SWCNode>& nodeSet) const {
    for (const auto& [id, node] : nodeSet) {
        if (node.pid != -1 && node.pid >= node.id) {
            //std::cerr << "Neuron is NOT! TOPOLOGICALLY sorted!: violation: node ID " << id << " has parent ID " << node.pid << std::endl;
            return false;
        }
    }
    return true;
}

bool NeuronGraph::hasSomaSegment(const std::map<int, SWCNode>& nodeSet) const {
    int somaCount = 0;
    for (const auto& [id, node] : nodeSet) {
        if (node.type == 1) {
            ++somaCount;
            if (somaCount > 1) return true;  // early exit
        }
    }
    return false;
}

bool NeuronGraph::isSomaMissing(const std::map<int, SWCNode>& nodeSet) const {
    for (const auto& [id, node] : nodeSet) {
        if (node.type == 1) {
            return false; // Soma is present
        }
    }
    return true; // No node of type 1 found
}