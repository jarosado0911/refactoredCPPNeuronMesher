#include "project/neurongraph.h"
#include <iostream>
#include <filesystem>

int main() {
	std::string swcPath = "/home/jarosado/repoRefactored/refactoredCPPNeuronMesher/data/SWC/228-10MG.CNG.swc";
	std::string ugxPath = "/home/jarosado/repoRefactored/refactoredCPPNeuronMesher/output/UGX/228-10MG.ugx";

	NeuronGraph graph;
	graph.readFromFile(swcPath);
	graph.writeToFileUGX(graph.getNodes(), ugxPath);

	std::cout << "[PASSED] converted " << swcPath << " to " << ugxPath << "\n";
	return 0;
}
