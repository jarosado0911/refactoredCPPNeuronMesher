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
			std::map<int, SWCNode> preprocessedNeuron;
        	if (this->isSomaMissing(nodeSet)) preprocessedNeuron=this->setSoma(nodeSet);
			if (this->hasSomaSegment(nodeSet)) preprocessedNeuron=this->removeSomaSegment(nodeSet);
			return preprocessedNeuron;
}

void NeuronGraph::readFromFile(const std::string& filename){
	nodes.clear(); // clear the nodes
	edges.clear(); // clear the edges
	
	std::ifstream infile(filename);
	if (!infile.is_open()){
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}

	std::string line;
	while(std::getline(infile,line)) {
	    if(line.empty() || line[0] == '#') continue;

	    std::istringstream iss(line);
	    SWCNode node;

	    if ( !(iss >> node.id >> node.type >> node.x >> node.y >> node.z >> node.radius >> node.pid)) {
	    	std::cerr << "Malformed line: " << line << std::endl;
		continue;
	    }
	    addNode(node);
	}
	std::cout << "Read SWC ..." << filename << std::endl;
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