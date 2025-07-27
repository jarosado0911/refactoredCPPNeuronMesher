/**
 * @file neurongraph.cpp
 * @brief Implementation of the NeuronGraph class for neuron morphology analysis and processing
 * @author CPPNeuronMesher Project
 * @date 2024
 * 
 * This file contains the core implementation of the NeuronGraph class, which provides
 * functionality for loading, processing, and analyzing neuron morphology data in SWC format.
 * It includes methods for topology validation, soma processing, and file I/O operations.
 */

#include "neurongraph.h"
#include "neuronugx.cpp"
#include "neuronoperations.cpp"
#include "neurontrunks.cpp"
#include <tinyxml2.h>

namespace fs = std::filesystem;

/**
 * @brief Constructor that initializes a NeuronGraph from a set of SWC nodes
 * @param nodeSet A map containing SWC nodes indexed by their IDs
 * 
 * This constructor creates a new NeuronGraph instance by clearing any existing
 * data and populating the graph with the provided nodes. It automatically
 * builds the parent-child relationships between nodes.
 * 
 * @note The nodeSet parameter uses structured binding, so the key is ignored (_)
 * @see addNode(const SWCNode& node)
 */
NeuronGraph::NeuronGraph(const std::map<int, SWCNode>& nodeSet){
	nodes.clear();
	edges.clear();

	for(auto& [_,node] : nodeSet){
		addNode(node);
	}
}

/**
 * @brief Sets the nodes of the graph, replacing any existing nodes
 * @param nodeSet A map containing SWC nodes indexed by their IDs
 * 
 * This method clears the current graph data and repopulates it with the
 * provided node set. It rebuilds all parent-child relationships from scratch.
 * 
 * @note This is equivalent to reconstructing the graph with new data
 * @see addNode(const SWCNode& node)
 */
void NeuronGraph::setNodes(const std::map<int,SWCNode>& nodeSet){
	nodes.clear();
	edges.clear();

	for(auto& [_,node] : nodeSet){
		addNode(node);
	}
}

/**
 * @brief Adds a single node to the graph and updates edge relationships
 * @param node The SWCNode to be added to the graph
 * 
 * This method adds a node to the internal nodes map and automatically
 * creates the parent-child edge relationship if the node has a valid parent
 * (pid != -1). Root nodes (pid == -1) are added without creating edges.
 * 
 * @note This method assumes node IDs are unique and will overwrite existing nodes
 * @see SWCNode structure for node data format
 */
void NeuronGraph::addNode(const SWCNode& node){
	nodes[node.id] = node;
	if(node.pid != -1) {
	   edges[node.pid].push_back(node.id);
	}
}

/**
 * @brief Preprocesses a node set to ensure proper soma properties
 * @param nodeSet The input map of SWC nodes to preprocess
 * @return A preprocessed map of SWC nodes with corrected soma properties
 * 
 * This method performs essential preprocessing steps on neuron data:
 * 1. Checks if soma is missing and adds one if necessary
 * 2. Removes soma segments if multiple soma nodes exist
 * 
 * The preprocessing ensures that the neuron has exactly one soma node,
 * which is critical for proper neuron analysis and mesh generation.
 * 
 * @see isSomaMissing(const std::map<int, SWCNode>& nodeSet)
 * @see hasSomaSegment(const std::map<int, SWCNode>& nodeSet)
 * @see setSoma(const std::map<int, SWCNode>& nodeSet)
 * @see removeSomaSegment(const std::map<int, SWCNode>& nodeSet)
 */
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

/**
 * @brief Reads neuron morphology data from an SWC format file
 * @param filename Path to the SWC file to read
 * 
 * This method parses an SWC (Standardized Morphology Format) file and populates
 * the graph with neuron nodes. The SWC format contains one node per line with
 * the following columns: ID, type, x, y, z, radius, parent_ID.
 * 
 * The parser handles:
 * - Comment lines (starting with #)
 * - Inline comments
 * - Whitespace normalization
 * - Tab-to-space conversion
 * - Error reporting for malformed lines
 * 
 * @note Clears existing graph data before loading new data
 * @note Prints summary information upon successful completion
 * @warning If file cannot be opened, an error message is printed and method returns
 * 
 * @see addNode(const SWCNode& node) for individual node processing
 */
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


/**
 * @brief Writes neuron morphology data to an SWC format file
 * @param nodeSet Map of SWC nodes to write to file
 * @param filename Output file path for the SWC data
 * 
 * This method exports neuron morphology data in the standard SWC format.
 * Nodes are written in sorted order by ID to ensure consistent output.
 * Each line contains: ID, type, x, y, z, radius, parent_ID.
 * 
 * The output format is compatible with standard neuron analysis tools
 * and can be read back using readFromFile().
 * 
 * @note If output file cannot be opened, an error message is printed and method returns
 * @note Prints confirmation message upon successful completion
 * 
 * @see readFromFile(const std::string& filename) for reading SWC files
 */
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

/**
 * @brief Automatically detects file format and reads neuron data
 * @param filename Path to the input file (SWC or UGX format)
 * 
 * This method automatically determines the file format based on the file
 * extension and calls the appropriate reader method:
 * - .swc files are read using readFromFile()
 * - .ugx files are read using readFromFileUGX()
 * 
 * This provides a convenient unified interface for loading neuron data
 * regardless of the input format.
 * 
 * @warning Prints error message for unsupported file extensions
 * 
 * @see readFromFile(const std::string& filename) for SWC format
 * @see readFromFileUGX(const std::string& filename) for UGX format
 */
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

/**
 * @brief Converts an SWC file to UGX format
 * @param inputfile Path to the input SWC file
 * @param outputfile Path to the output UGX file
 * 
 * This static utility method provides convenient SWC to UGX format conversion.
 * It creates a temporary NeuronGraph instance, loads the SWC data, and exports
 * it in UGX format suitable for mesh generation and finite element analysis.
 * 
 * @note This is a static method that can be called without a NeuronGraph instance
 * 
 * @see ugx2swc(const std::string& inputfile, const std::string& outputfile) for reverse conversion
 * @see writeToFileUGX() for UGX output functionality
 */
void NeuronGraph::swc2ugx(const std::string& inputfile, const std::string& outputfile)
{
	NeuronGraph graph(inputfile);
	auto nodes = graph.getNodes();
	graph.writeToFileUGX(nodes,outputfile);
}

/**
 * @brief Converts a UGX file to SWC format
 * @param inputfile Path to the input UGX file
 * @param outputfile Path to the output SWC file
 * 
 * This static utility method provides convenient UGX to SWC format conversion.
 * It creates a temporary NeuronGraph instance, loads the UGX data, and exports
 * it in standard SWC format for compatibility with neuron analysis tools.
 * 
 * @note This is a static method that can be called without a NeuronGraph instance
 * 
 * @see swc2ugx(const std::string& inputfile, const std::string& outputfile) for reverse conversion
 * @see writeToFile() for SWC output functionality
 */
void NeuronGraph::ugx2swc(const std::string& inputfile, const std::string& outputfile)
{
	NeuronGraph graph(inputfile);
	auto nodes = graph.getNodes();
	graph.writeToFile(nodes,outputfile);
}

/**
 * @brief Checks if the neuron nodes are topologically sorted
 * @param nodeSet Map of SWC nodes to check
 * @return true if nodes are topologically sorted, false otherwise
 * 
 * This method verifies that all parent nodes have IDs smaller than their
 * children nodes (parent_id < child_id). This property is important for
 * many neuron analysis algorithms and ensures proper tree traversal.
 * 
 * A topologically sorted neuron means that when nodes are processed in
 * ID order, all parent nodes are encountered before their children.
 * 
 * @note Root nodes (pid == -1) are always considered valid
 * @note The commented debug output can be uncommented for troubleshooting
 * 
 * @see SWCNode structure for node ID and parent ID fields
 */
bool NeuronGraph::isTopologicallySorted(const std::map<int, SWCNode>& nodeSet) const {
    for (const auto& [id, node] : nodeSet) {
        if (node.pid != -1 && node.pid >= node.id) {
            //std::cerr << "Neuron is NOT! TOPOLOGICALLY sorted!: violation: node ID " << id << " has parent ID " << node.pid << std::endl;
            return false;
        }
    }
    return true;
}

/**
 * @brief Checks if the neuron has multiple soma nodes (soma segment)
 * @param nodeSet Map of SWC nodes to check
 * @return true if multiple soma nodes exist, false otherwise
 * 
 * This method detects the presence of soma segments, which occur when
 * multiple nodes have type 1 (soma). In proper neuron morphology,
 * there should be exactly one soma node. Multiple soma nodes can
 * cause issues in analysis and mesh generation.
 * 
 * The method uses early exit optimization - it returns true as soon
 * as a second soma node is found, avoiding unnecessary iterations.
 * 
 * @note Soma nodes are identified by type == 1 in SWC format
 * @note This is used in preprocessing to clean up neuron data
 * 
 * @see preprocess() for automatic soma segment removal
 * @see removeSomaSegment() for soma segment correction
 */
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

/**
 * @brief Checks if the neuron is missing a soma node
 * @param nodeSet Map of SWC nodes to check
 * @return true if no soma node exists, false if soma is present
 * 
 * This method determines whether the neuron morphology lacks a soma
 * (cell body). Every valid neuron should have exactly one soma node
 * of type 1. Missing somas can occur in incomplete reconstructions
 * or when dealing with neuron fragments.
 * 
 * The method uses early exit - it returns false as soon as any
 * soma node is found, making it efficient for large neuron datasets.
 * 
 * @note Soma nodes are identified by type == 1 in SWC format
 * @note This is used in preprocessing to add missing somas
 * 
 * @see preprocess() for automatic soma addition
 * @see setSoma() for adding missing soma nodes
 */
bool NeuronGraph::isSomaMissing(const std::map<int, SWCNode>& nodeSet) const {
    for (const auto& [id, node] : nodeSet) {
        if (node.type == 1) {
            return false; // Soma is present
        }
    }
    return true; // No node of type 1 found
}