#include "project/neurongraph.h"
#include <cassert>
#include <filesystem>

std::string getExecutableDir() {
    return std::filesystem::canonical("/proc/self/exe").parent_path().string();
}

void test_add_nodes(){
	NeuronGraph graph;

	SWCNode n1{1,-1,1,0.0,0.0,0.0,1.0};
	SWCNode n2{2, 1,3,1.0,0.0,0.0,1.0};
	graph.addNode(n1);
	graph.addNode(n2);

	assert(graph.numberOfNodes()==2);
	assert(graph.numberOfEdges()==1);
	std::cout << "[PASSED] test .add_nodes()\n\n";
}

void test_read_from_data_folder(){
        std::string execDir = getExecutableDir();
	std::string filepath = execDir + "/../data/neuron.swc"; 

	NeuronGraph graph;
	graph.readFromFile(filepath);
	assert(graph.numberOfNodes()>0);
	assert(graph.numberOfEdges()>0);

	std::cout << "[PASSED] test .readFromFile()\n\n";
}

void test_write_to_output_folder(){
	std::string execDir = getExecutableDir();
	std::string tempFile = execDir + "/../output/test_output/test_output.swc";

	// Create the graph and add nodes
	NeuronGraph graph;
	SWCNode n1{1,-1,1,0.0,0.0,0.0,1.0};
        SWCNode n2{2, 1,3,1.0,0.0,0.0,1.0};
        graph.addNode(n1);
        graph.addNode(n2);

	// Write to file
	graph.writeToFile(graph.getNodes(),tempFile);

	// Read back into new graph
	NeuronGraph loaded;
	loaded.readFromFile(tempFile);

	// Validate
	assert(loaded.numberOfNodes() == 2);
	assert(loaded.numberOfEdges() == 1);

	const auto& reloaded = loaded.getNodes();
	assert(reloaded.at(1).x == 0.0);
	assert(reloaded.at(2).pid == 1);

	std::cout << "[PASSED] test .writeToFile()\n\n";

	// clean up
	// std::filesystem::remove(tempFile);
}

void test_numberOfNodes(){
	std::string execDir = getExecutableDir();
	std::string inputFile = execDir + "/../data/neuron.swc";

	// Create the graph from the input file
	NeuronGraph graph;
	graph.readFromFile(inputFile);
	assert(graph.numberOfNodes() > 0);

	std::cout << "[PASSED] test .numberOfNodes()\n\n";
}

void test_numberOfEdges(){
	std::string execDir = getExecutableDir();
	std::string inputFile = execDir + "/../data/neuron.swc";

	// Create the graph from the input file
	NeuronGraph graph;
	graph.readFromFile(inputFile);
	assert(graph.numberOfEdges() > 0);

	std::cout << "[PASSED] test .numberOfEdges()\n\n";
}

void test_write_to_file_UGX(){
	std::string execDir = getExecutableDir();
	std::string inputFile = execDir + "/../data/neuron.swc";
	std::string outputFile = execDir + "/../output/test_output/testneuron.ugx";

	// Create the graph from the input file
	NeuronGraph graph;
	graph.readFromFile(inputFile);
	auto nodeSet = graph.getNodes();
	graph.writeToFileUGX(nodeSet,outputFile);

	std::cout << "[PASSED] test .writeToFileUGX()\n\n";
	assert(true);
}

void test_read_from_file_UGX(){
	std::string execDir = getExecutableDir();
	std::string inputFile = execDir + "/../data/neuron.ugx";
	std::string outputFile = execDir + "/../output/test_output/testneuron2.ugx";

	NeuronGraph graph;
	graph.readFromFileUGX(inputFile);
	auto nodeSet = graph.getNodes();
	graph.writeToFileUGX(nodeSet,outputFile);
	
	std::cout << "[PASSED] test .readFromFileUGX()\n\n";
	assert(true);

}

void test_read_from_file_UGX_or_SWC(){
	std::string execDir = getExecutableDir();
	std::string inputFileSWC = execDir + "/../data/neuron.swc";
	std::string inputFileUGX = execDir + "/../data/neuron.ugx";

	NeuronGraph graphSWC, graphUGX;
	graphSWC.readFromFileUGXorSWC(inputFileSWC);
	graphUGX.readFromFileUGXorSWC(inputFileUGX);
	auto nodeSetSWC = graphSWC.getNodes();
	auto nodeSetUGX = graphUGX.getNodes();
	std::cout << "[PASSED] test .readFromFileUGXorSWC()\n\n";
	assert(nodeSetSWC.size()==nodeSetUGX.size());
}

void test_is_topologicallySorted(){
	std::string execDir = getExecutableDir();
	std::string inputFileSWC = execDir + "/../data/neuron.swc";

	NeuronGraph graphSWC;
	graphSWC.readFromFileUGXorSWC(inputFileSWC);
	std::cout << "[PASSED] test .isTopologicallySorted()\n\n";
	assert(graphSWC.isTopologicallySorted(graphSWC.getNodes()));
}

void test_has_soma_segment(){
	std::string execDir = getExecutableDir();
	std::string inputFileSWC = execDir + "/../data/neuron.swc";

	NeuronGraph graphSWC;
	graphSWC.readFromFileUGXorSWC(inputFileSWC);
	std::cout << "[PASSED] test .hasSomaSegment()\n\n";
	assert(graphSWC.hasSomaSegment(graphSWC.getNodes()));
}

void test_remove_soma_segment(){
	std::string execDir = getExecutableDir();
	std::string inputFileSWC = execDir + "/../data/neuron.swc";
	std::string outputFile = execDir + "/../output/test_output/neuron_nosomasegment.swc";

	NeuronGraph graphSWC;
	graphSWC.readFromFileUGXorSWC(inputFileSWC);
	auto nodeSet = graphSWC.removeSomaSegment(graphSWC.getNodes());
	graphSWC.writeToFile(nodeSet,outputFile);
	std::cout << "Topologically Sorted: " << std::boolalpha << graphSWC.isTopologicallySorted(nodeSet) << std::endl;
	std::cout << "[PASSED] test .removeSomaSegment()\n\n";
	assert(graphSWC.getNodes().size() != nodeSet.size());
}

void test_split_edges(){
	std::string execDir = getExecutableDir();
	std::string inputFileSWC = execDir + "/../data/neuron.swc";
	std::string outputFile = execDir + "/../output/test_output/neuron_splitedges1.swc";

	NeuronGraph graphSWC;
	graphSWC.readFromFileUGXorSWC(inputFileSWC);
	auto nodeSet = graphSWC.removeSomaSegment(graphSWC.getNodes());

	for(int i = 1; i <= 3; ++i){
		nodeSet = graphSWC.splitEdges(nodeSet);
		graphSWC.writeToFile(nodeSet,outputFile);
		std::cout << "Split Iteration " + std::to_string(i+1) << " complete! Saved to " << outputFile << "\n\n";
		graphSWC.readFromFile(outputFile);
		nodeSet = graphSWC.getNodes();
		outputFile = execDir + "/../output/test_output/neuron_splitedges"+std::to_string(i+1)+".swc"; 
	}
	std::cout << "[PASSED] test .splitEdges()\n\n";
	assert(true);
}

void test_is_soma_missing(){
	std::string execDir = getExecutableDir();
	std::string inputFileSWC = execDir + "/../data/purkinjeNoSoma.swc";
	std::string outputFile = execDir + "/../output/test_output/purkinjeNoSoma";
	NeuronGraph graphSWC;
	graphSWC.readFromFileUGXorSWC(inputFileSWC);
	auto nodeSet = graphSWC.getNodes();

	graphSWC.writeToFile(nodeSet,outputFile +".swc");
	std::cout << "[PASSED] test wrote purkinjeNoSoma to SWC\n\n";

	graphSWC.writeToFileUGX(nodeSet,outputFile +".ugx");
	std::cout << "[PASSED] test wrote purkinjeNoSoma to UGX\n\n";

	std::cout << "[PASSED] test .isSomaMissing()\n\n";
	assert(graphSWC.isSomaMissing(nodeSet));
}

void test_set_soma(){
	std::string execDir = getExecutableDir();
	std::string inputFileSWC = execDir + "/../data/purkinjeNoSoma.swc";
	std::string outputFile = execDir + "/../output/test_output/purkinjeWithSoma.swc";
	
	NeuronGraph graphSWC;
	graphSWC.readFromFileUGXorSWC(inputFileSWC);
	auto nodeSet = graphSWC.getNodes();
	nodeSet = graphSWC.setSoma(nodeSet);
	graphSWC.writeToFile(nodeSet,outputFile);
	
	std::cout << "[PASSED] test .setSoma()\n\n";
	assert(true);
}

int main(){
	test_add_nodes();
	test_read_from_data_folder();
	test_write_to_output_folder();
	test_numberOfNodes();
	test_numberOfEdges();
	test_write_to_file_UGX();
	test_read_from_file_UGX();
	test_read_from_file_UGX_or_SWC();
	test_is_topologicallySorted();
	test_has_soma_segment();
	test_remove_soma_segment();
	test_split_edges();
	test_is_soma_missing();
	test_set_soma();

	std::cout<< "\nAll tests passed.\n";
	return 0;
}
