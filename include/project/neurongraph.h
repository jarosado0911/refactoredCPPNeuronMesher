/**
 * @file neurongraph.h
 * @brief Definition of the NeuronGraph class for handling neuron morphology data
 * 
 * This header defines the NeuronGraph class which provides functionality for
 * reading, writing, and processing neuron morphology data in SWC and UGX formats.
 * It includes methods for topological sorting, soma processing, and geometry
 * manipulation of neuron structures.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 */

#ifndef NEURONGRAPH_H
#define NEURONGRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <cmath>
#include <numeric>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <filesystem>

/**
 * @brief Structure representing a single node in an SWC neuron morphology
 * 
 * This structure holds the essential properties of a neuron morphology node
 * as defined in the SWC (Single-Write-Cable) format. Each node represents
 * a 3D point in the neuron structure with associated metadata.
 * 
 * @see http://www.neuronland.org/NLMorphologyConverter/MorphologyFormats/SWC/Spec.html
 */
struct SWCNode {
    /** @brief Unique identifier for the node */
    int id;
    
    /** @brief Parent node ID (-1 for root nodes) */
    int pid;
    
    /** 
     * @brief Node type identifier
     * 
     * Standard SWC types:
     * - 0: undefined
     * - 1: soma
     * - 2: axon
     * - 3: basal dendrite
     * - 4: apical dendrite
     * - 5-7: custom types (user-defined)
     */
    int type;
    
    /** @brief X-coordinate in 3D space */
    double x;
    
    /** @brief Y-coordinate in 3D space */
    double y;
    
    /** @brief Z-coordinate in 3D space */
    double z;
    
    /** @brief Radius of the neural process at this node */
    double radius;
};

/**
 * @brief Class for representing and processing neuron morphology graphs
 * 
 * The NeuronGraph class provides comprehensive functionality for working with
 * neuron morphology data, including file I/O, topological analysis, and geometric
 * processing. It supports both SWC and UGX file formats and includes methods
 * for manipulating and analyzing neuron structures.
 * 
 * Key features:
 * - Reading and writing SWC/UGX files
 * - Topological sorting and validation
 * - Soma detection and processing
 * - Edge splitting and resampling
 * - Trunk extraction and manipulation
 */
class NeuronGraph {
	private:
	    /** @brief Map of node IDs to SWCNode objects */
	    std::map<int,SWCNode> nodes;
	    
	    /** @brief Adjacency list representation of the neuron graph */
	    std::map<int, std::vector<int>> edges;

	    /**
	     * @brief Builds a neighbor map from a set of nodes
	     * @param[in] nodeSet Set of nodes to process
	     * @return Map where each key is a node ID and the value is a list of neighbor node IDs
	     * 
	     * This helper function constructs an adjacency list representation of the graph
	     * from a set of nodes by establishing parent-child relationships.
	     */
	    std::map<int, std::vector<int>> getNeighborMap(const std::map<int, SWCNode>& nodeSet) const;
	public:
	    /**
	     * @brief Default constructor
	     * 
	     * Creates an empty NeuronGraph with no nodes or edges.
	     */
	    NeuronGraph() {nodes.clear(); edges.clear();};
	    
	    /**
	     * @brief Constructor that loads a neuron from file
	     * @param[in] filename Path to the SWC or UGX file to load
	     * 
	     * @see readFromFileUGXorSWC()
	     */
	    NeuronGraph(std::string filename) {this->readFromFileUGXorSWC(filename);};
	    
	    /**
	     * @brief Constructs a NeuronGraph from an existing set of nodes
	     * @param[in] nodeSet Map of node IDs to SWCNode objects
	     */
	    NeuronGraph(const std::map<int, SWCNode>& nodeSet);

	    /**
	     * @brief Adds a single node to the graph
	     * @param[in] node The SWCNode to add
	     * 
	     * @note This method is typically used internally by other methods
	     *       that read or generate node data.
	     */
	    void addNode(const SWCNode &node);

		/**
		 * @brief Replaces the current set of nodes in the graph
		 * @param[in] nodeSet New set of nodes to use
		 * 
		 * This method clears the current graph and rebuilds it using the provided nodes.
		 * It automatically updates the internal adjacency list.
		 */
		void setNodes(const std::map<int,SWCNode>& nodeSet);

		/**
		 * @brief Checks if a set of nodes is topologically sorted
		 * @param[in] nodeSet The set of nodes to check
		 * @return true if the nodes are topologically sorted, false otherwise
		 * 
		 * A set of nodes is considered topologically sorted if for every node,
		 * its parent ID is less than its own ID (i.e., parent nodes come before
		 * their children in the sequence).
		 */
		bool isTopologicallySorted(const std::map<int, SWCNode>& nodeSet) const;

		/**
		 * @overload
		 * Checks if the current graph's nodes are topologically sorted
		 */
		bool isTopologicallySorted() const {return this->isTopologicallySorted(this->nodes);};

		/**
		 * @brief Sorts nodes topologically using Kahn's algorithm
		 * @param[in] nodeSet The set of nodes to sort
		 * @return A new map of nodes sorted in topological order
		 * 
		 * This method ensures that parent nodes always appear before their children
		 * in the resulting sequence, which is important for many processing steps.
		 * 
		 * @note If the graph contains cycles, the behavior is undefined.
		 * @see https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm
		 */
		std::map<int, SWCNode> topologicalSort(const std::map<int, SWCNode>& nodeSet) const;

		/**
		 * @overload
		 * Sorts the current graph's nodes topologically
		 */
		std::map<int, SWCNode> topologicalSort() const {return this->topologicalSort(this->nodes);};

		/**
		 * @brief Checks if the node set contains a soma segment
		 * @param[in] nodeSet The set of nodes to check
		 * @return true if a soma segment is found, false otherwise
		 * 
		 * A soma segment is defined as multiple connected nodes of type 1 (soma).
		 */
		bool hasSomaSegment(const std::map<int, SWCNode>& nodeSet) const;

		/**
		 * @overload
		 * Checks if the current graph contains a soma segment
		 */
		bool hasSomaSegment() const {return this->hasSomaSegment(this->nodes);};

		/**
		 * @brief Checks if the node set is missing a soma
		 * @param[in] nodeSet The set of nodes to check
		 * @return true if no soma is present, false otherwise
		 * 
		 * This is the inverse of hasSomaSegment() and is provided for readability.
		 */
		bool isSomaMissing(const std::map<int, SWCNode>& nodeSet) const;

		/**
		 * @overload
		 * Checks if the current graph is missing a soma
		 */
		bool isSomaMissing() const {return this->isSomaMissing(this->nodes);};

		/**
		 * @brief Removes the soma segment from a set of nodes
		 * @param[in] inputNodes The set of nodes to process
		 * @return A new set of nodes with the soma segment removed
		 * 
		 * This method identifies and removes the soma segment (multiple connected type 1 nodes)
		 * and replaces it with a single representative soma node at the average position.
		 */
		std::map<int, SWCNode> removeSomaSegment(const std::map<int, SWCNode>& inputNodes) const;

		/**
		 * @overload
		 * Removes the soma segment from the current graph
		 */
		std::map<int, SWCNode> removeSomaSegment() const {return this->removeSomaSegment(this->nodes);};

		/**
		 * @brief Adds a soma node if none exists
		 * @param[in] nodeSet The set of nodes to process
		 * @return A new set of nodes with a soma node added if needed
		 * 
		 * If no soma (type 1) node is present, this method adds one at the root position
		 * of the neuron tree.
		 */
		std::map<int, SWCNode> setSoma(const std::map<int, SWCNode>& nodeSet) const;

		/**
		 * @overload
		 * Adds a soma node to the current graph if none exists
		 */
		std::map<int, SWCNode> setSoma() const {return this->setSoma(this->nodes);};

		/**
		 * @brief Applies standard preprocessing steps to a set of nodes
		 * @param[in] nodeSet The set of nodes to preprocess
		 * @return A new set of preprocessed nodes
		 * 
		 * Preprocessing includes:
		 * 1. Topological sorting
		 * 2. Soma segment handling
		 * 3. Soma addition if missing
		 */
		std::map<int, SWCNode> preprocess(const std::map<int, SWCNode> & nodeSet) const;

	    /**
	     * @brief Reads neuron data from an SWC file
	     * @param[in] filename Path to the SWC file to read
	     * 
	     * @throws std::runtime_error If the file cannot be opened or parsed
	     * @see http://www.neuronland.org/NLMorphologyConverter/MorphologyFormats/SWC/Spec.html
	     */
	    void readFromFile(const std::string& filename);

	    /**
	     * @brief Writes a set of nodes to an SWC file
	     * @param[in] nodeSet The nodes to write
	     * @param[in] filename Path to the output file
	     * 
	     * @note The file will be overwritten if it already exists
	     */
	    void writeToFile(const std::map<int,SWCNode> & nodeSet,
			     const std::string& filename);

	    /**
	     * @overload
	     * Writes the current graph's nodes to an SWC file
	     */
	    void writeToFile(const std::string& filename) {this->writeToFile(this->nodes, filename);};

	    /**
	     * @brief Reads neuron data from a UGX file
	     * @param[in] filename Path to the UGX file to read
	     * 
	     * @throws std::runtime_error If the file cannot be opened or parsed
	     * @note UGX is an XML-based format used by the UG4 simulation framework
	     */
	    void readFromFileUGX(const std::string& filename);

		/**
		 * @brief Reads a neuron file, automatically detecting the format
		 * @param[in] filename Path to the neuron file (SWC or UGX)
		 * 
		 * This method examines the file extension to determine whether to use
		 * readFromFile() or readFromFileUGX().
		 * 
		 * @throws std::runtime_error If the file format is not supported
		 */
		void readFromFileUGXorSWC(const std::string& filename);

		/**
		 * @brief Writes a set of nodes to a UGX file
		 * @param[in] nodeSet The nodes to write
		 * @param[in] filename Path to the output file
		 * 
		 * @note The file will be overwritten if it already exists
		 */
		void writeToFileUGX(const std::map<int,SWCNode> & nodeSet,
				const std::string& filename);

		/**
		 * @overload
		 * Writes the current graph's nodes to a UGX file
		 */
		void writeToFileUGX(const std::string& filename) {this->writeToFileUGX(this->nodes, filename);};

		/**
		 * @brief Converts an SWC file to UGX format
		 * @param[in] inputfile Path to the input SWC file
		 * @param[in] outputfile Path for the output UGX file
		 * 
		 * This is a convenience method that combines readFromFile() and writeToFileUGX().
		 */
		void swc2ugx(const std::string& inputfile, const std::string& outputfile);

		/**
		 * @brief Converts a UGX file to SWC format
		 * @param[in] inputfile Path to the input UGX file
		 * @param[in] outputfile Path for the output SWC file
		 * 
		 * This is a convenience method that combines readFromFileUGX() and writeToFile().
		 */
		void ugx2swc(const std::string& inputfile, const std::string& outputfile);

	    /**
	     * @brief Returns the number of nodes in the graph
	     * @return The total number of nodes
	     */
	    int numberOfNodes() {return this->nodes.size();};

	    /**
	     * @brief Returns the number of edges in the graph
	     * @return The total number of edges
	     */
	    int numberOfEdges() {return this->edges.size();};

	    /**
	     * @brief Returns a copy of all nodes in the graph
	     * @return A map of node IDs to SWCNode objects
	     */
	    auto getNodes() { return this->nodes; };

		/**
		 * @brief Splits all edges in the graph by inserting midpoint nodes
		 * @param[in] nodeSet The set of nodes to process
		 * @return A new set of nodes with edges split
		 * 
		 * This method inserts a new node at the midpoint of each edge,
		 * effectively doubling the number of edges while preserving the
		 * overall shape of the neuron.
		 */
		std::map<int, SWCNode> splitEdges(const std::map<int, SWCNode>& nodeSet) const;

		/**
		 * @overload
		 * Splits all edges in the current graph
		 */
		std::map<int, SWCNode> splitEdges() const {return this->splitEdges(this->nodes);};

		/**
		 * @brief Applies edge splitting N times recursively
		 * @param[in] nodeSet The set of nodes to process
		 * @param[in] N Number of times to split the edges
		 * @return A vector of node sets, where each element represents the result
		 *         of i+1 splits (i = 0 to N-1)
		 */
		std::vector<std::map<int, SWCNode>> splitEdgesN(const std::map<int, SWCNode>& nodeSet, int N) const;

		/**
		 * @overload
		 * Applies edge splitting N times to the current graph
		 */
		std::vector<std::map<int, SWCNode>> splitEdgesN(int N) const {return this->splitEdgesN(this->nodes, N);};

		/**
		 * @brief Extracts trunk segments from a neuron morphology
		 * @param[in] nodeSet The set of nodes to process
		 * @param[in] resetIndex If true, renumbers node IDs sequentially
		 * @return A map where each key is a trunk ID and the value is a map of node IDs to SWCNodes
		 * 
		 * A trunk is defined as a linear segment between branch points or between
		 * a branch point and a leaf node.
		 */
		std::map<int,std::map<int,SWCNode>> getTrunks(const std::map<int,SWCNode>& nodeSet, bool resetIndex = false) const;

		/**
		 * @overload
		 * Extracts trunk segments from the current graph
		 */
		std::map<int,std::map<int,SWCNode>> getTrunks(bool resetIndex = false) const {return this->getTrunks(this->nodes, resetIndex);};

		/**
		 * @brief Creates a mapping from trunk IDs to their parent trunk IDs
		 * @param[in] nodeSet The original set of nodes
		 * @param[in] trunkNodeSets Map of trunk IDs to their nodes
		 * @return A map where keys are trunk IDs and values are parent trunk IDs (-1 for root trunks)
		 */
		std::map<int, int> getTrunkParentMap(const std::map<int, SWCNode>& nodeSet,
			                                 const std::map<int, std::map<int, SWCNode>>& trunkNodeSets) const;

		/**
		 * @brief Combines multiple trunks into a single node set
		 * @param[in] trunkNodeSets Map of trunk IDs to their nodes
		 * @return A single map containing all nodes from all trunks
		 * 
		 * @note This version preserves the original node IDs
		 */
		std::map<int, SWCNode> assembleTrunks(const std::map<int, std::map<int, SWCNode>>& trunkNodeSets) const;

		/**
		 * @brief Combines resampled trunks using a parent-child relationship map
		 * @param[in] resampledTrunks Map of trunk IDs to their resampled nodes
		 * @param[in] trunkParentMap Map of trunk IDs to their parent trunk IDs
		 * @return A single map containing all nodes from all trunks with updated parent references
		 * 
		 * This version is used when trunk nodes have been resampled and need to be
		 * reconnected based on the original topology.
		 */
		std::map<int, SWCNode> assembleTrunks(const std::map<int, std::map<int, SWCNode>>& resampledTrunks,
											  const std::map<int,int>& trunkParentMap);

		/**
		 * @brief Resamples a single trunk using linear interpolation
		 * @param[in] trunk The trunk to resample (map of node IDs to SWCNodes)
		 * @param[in,out] delta Target spacing between resampled points (may be adjusted)
		 * @return A new set of nodes representing the resampled trunk
		 */
		std::map<int, SWCNode> linearSplineResampleTrunk(const std::map<int, SWCNode>& trunk, double& delta) const;

		/**
		 * @brief Resamples all trunks using linear interpolation
		 * @param[in,out] trunks Map of trunk IDs to their nodes
		 * @param[in,out] delta Target spacing between resampled points (may be adjusted)
		 * @return A new map of resampled trunks
		 */
		std::map<int, std::map<int, SWCNode>> allLinearSplineResampledTrunks(std::map<int, std::map<int, SWCNode>>& trunks, double& delta) const;

		/**
		 * @brief Resamples a single trunk using cubic spline interpolation
		 * @param[in] trunk The trunk to resample (map of node IDs to SWCNodes)
		 * @param[in,out] delta Target spacing between resampled points (may be adjusted)
		 * @return A new set of nodes representing the resampled trunk
		 * 
		 * Cubic spline interpolation generally produces smoother curves than linear
		 * interpolation, especially for coarsely sampled data.
		 */
		std::map<int, SWCNode> cubicSplineResampleTrunk(const std::map<int, SWCNode>& trunk, double& delta) const;

		/**
		 * @brief Resamples all trunks using cubic spline interpolation
		 * @param[in,out] trunks Map of trunk IDs to their nodes
		 * @param[in,out] delta Target spacing between resampled points (may be adjusted)
		 * @return A new map of resampled trunks
		 */
		std::map<int, std::map<int, SWCNode>> allCubicSplineResampledTrunks(std::map<int, std::map<int, SWCNode>>& trunks, double& delta) const;

		/**
		 * @brief Generates multiple levels of refined neuron morphologies
		 * @param[in] nodeSet The set of nodes to refine
		 * @param[in,out] delta Initial spacing parameter for refinement
		 * @param[in,out] N Number of refinement levels to generate
		 * @param[in,out] method Refinement method ("linear" or "cubic")
		 * @return A map where keys are refinement levels and values are the refined node sets
		 * 
		 * This method generates a series of increasingly refined versions of the input
		 * neuron morphology by repeatedly applying the specified interpolation method.
		 */
		std::map<int, std::map<int,SWCNode>> generateRefinements(const std::map<int,SWCNode>& nodeSet, 
																 double& delta, 
																 int& N, 
																 std::string& method);

		/**
		 * @overload
		 * Generates refinements for the current graph's nodes
		 */
		std::map<int, std::map<int,SWCNode>> generateRefinements(double& delta, 
																 int& N, 
																 std::string& method) {
			return this->generateRefinements(this->nodes, delta, N, method); 
		};
};

#endif // NEURONGRAPH_H
