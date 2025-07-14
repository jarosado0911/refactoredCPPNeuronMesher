#include "project/neurongraph.h"
#include <iostream>
#include <filesystem>

int main() {
	std::string swcPath = "@SWC_FILE@";
	std::string ugxPath = "@UGX_FILE@";

	NeuronGraph graph;
	graph.readFromFile(swcPath);
	graph.writeToFileUGX(graph.getNodes(), ugxPath);

	std::cout << "[PASSED] converted " << swcPath << " to " << ugxPath << "\n";
	return 0;
}
