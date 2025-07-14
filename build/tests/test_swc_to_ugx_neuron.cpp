#include "project/neurongraph.h"
#include <iostream>
#include <filesystem>

int main() {
	std::string swcPath = "/home/jarosado/repoRefactored/refactoredCPPNeuronMesher/data/SWC/neuron.swc";
	std::string ugxPath = "/home/jarosado/repoRefactored/refactoredCPPNeuronMesher/output/UGX/neuron.ugx";

	NeuronGraph graph;
	graph.readFromFile(swcPath);
	graph.writeToFileUGX(graph.getNodes(), ugxPath);

	std::cout << "[PASSED] converted " << swcPath << " to " << ugxPath << "\n";
	return 0;
}
