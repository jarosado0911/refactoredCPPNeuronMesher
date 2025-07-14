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

// Structure to hold SWC node information
struct SWCNode {
	int id, pid;
	int type;
	double x,y,z;
	double radius;
};

class NeuronGraph {
	private:
	    std::map<int,SWCNode> nodes;
	    std::map<int, std::vector<int>> edges;

		// get neighbor map
		std::map<int, std::vector<int>> getNeighborMap(const std::map<int, SWCNode>& nodeSet) const;
	public:

		// constructors
		NeuronGraph(){nodes.clear(); edges.clear();};
		NeuronGraph(std::string filename){this->readFromFileUGXorSWC(filename);};
		NeuronGraph(const std::map<int, SWCNode>& nodeSet);

	    // internal method for adding a single node
	    void addNode(const SWCNode &node);

		// set nodes method
		void setNodes(const std::map<int,SWCNode>& nodeSet);

		// check topology
		bool isTopologicallySorted(const std::map<int, SWCNode>& nodeSet) const;
		bool isTopologicallySorted() const {return this->isTopologicallySorted(this->nodes);};

		// topological sort
		std::map<int, SWCNode> topologicalSort(const std::map<int, SWCNode>& nodeSet) const;
		std::map<int, SWCNode> topologicalSort() const {return this->topologicalSort(this->nodes);};

		// check for soma segment
		bool hasSomaSegment(const std::map<int, SWCNode>& nodeSet) const;
		bool hasSomaSegment() const {return this->hasSomaSegment(this->nodes);};

		bool isSomaMissing(const std::map<int, SWCNode>& nodeSet) const;
		bool isSomaMissing() const {return this->isSomaMissing(this->nodes);};

		// remove the soma segment
		std::map<int, SWCNode> removeSomaSegment(const std::map<int, SWCNode>& inputNodes) const;
		std::map<int, SWCNode> removeSomaSegment() const {return this->removeSomaSegment(this->nodes);};

		// set soma if no soma present
		std::map<int, SWCNode> setSoma(const std::map<int, SWCNode>& nodeSet) const;
		std::map<int, SWCNode> setSoma() const {return this->setSoma(this->nodes);};

		// preprocess neuron
		std::map<int, SWCNode> preprocess(const std::map<int, SWCNode> & nodeSet) const;

	    // method for reading an SWC file
	    void readFromFile(const std::string& filename);

	    // method for writing a set of nodes to a file
	    void writeToFile(const std::map<int,SWCNode> & nodeSet,
			     const std::string& filename);
		void writeToFile(const std::string& filename){this->writeToFile(this->nodes,filename);};

	    // method for reading from a UGX file
	    void readFromFileUGX(const std::string& filename);

		// method for reading from either UGX or SWC file
		void readFromFileUGXorSWC(const std::string& filename);

	    // method for writing to a UGX file
	    void writeToFileUGX(const std::map<int,SWCNode> & nodeSet,
			        const std::string& filename);
		void writeToFileUGX(const std::string& filename){this->writeToFileUGX(this->nodes,filename);};

		// convert SWC to UGX and UGX to SWC
		void swc2ugx(const std::string& inputfile, const std::string& outputfile);
		void ugx2swc(const std::string& inputfile, const std::string& outputfile);

	    // get methods
	    int numberOfNodes(){return this->nodes.size();};
	    int numberOfEdges(){return this->edges.size();};
	    auto getNodes(){ return this->nodes; };

		// split edges
		std::map<int, SWCNode> splitEdges(const std::map<int, SWCNode>& nodeSet) const;
		std::map<int, SWCNode> splitEdges() const {return this->splitEdges(this->nodes);};

		// split edges, N times
		std::vector<std::map<int, SWCNode>> splitEdgesN(const std::map<int, SWCNode>& nodeSet, int N) const;
		std::vector<std::map<int, SWCNode>> splitEdgesN(int N) const {return this->splitEdgesN(this->nodes,N);};

		// get the trunks
		std::map<int,std::map<int,SWCNode>> getTrunks(const std::map<int,SWCNode>& nodeSet, bool resetIndex = false) const;
		std::map<int,std::map<int,SWCNode>> getTrunks(bool resetIndex = false) const {return this->getTrunks(this->nodes, resetIndex);};

		// get trunk parent map
		std::map<int, int> getTrunkParentMap(const std::map<int, SWCNode>& nodeSet,
			                                 const std::map<int, std::map<int, SWCNode>>& trunkNodeSets) const;
		// assemble trunks using NON Reset Node Ids
		std::map<int, SWCNode> assembleTrunks(const std::map<int, std::map<int, SWCNode>>& trunkNodeSets) const;

		// assemble trunks that are RESET, need the trunkParentMap
		std::map<int, SWCNode> assembleTrunks(const std::map<int, std::map<int, SWCNode>>& resampledTrunks,
											  const std::map<int,int>& trunkParentMap);

		// linear resample trunks
		std::map<int, SWCNode> linearSplineResampleTrunk(const std::map<int, SWCNode>& trunk, double& delta) const;
		std::map<int, std::map<int, SWCNode>> allLinearSplineResampledTrunks(std::map<int, std::map<int, SWCNode>>& trunks, double& delta) const;
};

#endif // end of the neurongraph header file
