/**
 * @file neuronoperations.cpp
 * @brief Implementation of neuron morphology operations and transformations
 * @author CPPNeuronMesher Project
 * @date 2024
 * 
 * This file contains advanced operations for neuron morphology processing,
 * including soma segment handling, edge splitting, topological sorting,
 * and soma assignment operations.
 */

#include <unordered_map>
#include "neurongraph.h"

/**
 * @brief Removes multiple soma nodes and replaces them with a single averaged soma
 * @param inputNodes The input map of SWC nodes containing multiple soma nodes
 * @return A new map of SWC nodes with a single soma node at ID 1
 * 
 * This method processes neuron morphologies that have multiple soma nodes (soma segments)
 * and consolidates them into a single soma node. The process involves:
 * 1. Identifying all soma nodes (type == 1)
 * 2. Computing the average position and radius of all soma nodes
 * 3. Creating a new soma node with ID 1 at the averaged location
 * 4. Reassigning all non-soma nodes with new sequential IDs starting from 2
 * 5. Updating parent-child relationships to connect to the new soma
 * 6. Ensuring topological sorting of the result
 * 
 * @note If no soma nodes are found, returns the input unchanged
 * @note The resulting soma always has ID 1 and parent ID -1 (root)
 * @note All nodes originally connected to any soma will be connected to the new soma
 * 
 * @see hasSomaSegment() to detect if soma segments exist
 * @see topologicalSort() for ensuring proper node ordering
 */
std::map<int, SWCNode> NeuronGraph::removeSomaSegment(const std::map<int, SWCNode>& inputNodes) const {
    std::vector<int> somaIds;
    for (const auto& [id, node] : inputNodes) {
        if (node.type == 1) somaIds.push_back(id);
    }

    if (somaIds.empty()) return inputNodes;

    double x = 0, y = 0, z = 0, r = 0;
    for (int id : somaIds) {
        const SWCNode& n = inputNodes.at(id);
        x += n.x;
        y += n.y;
        z += n.z;
        r += n.radius;
    }
    x /= somaIds.size();
    y /= somaIds.size();
    z /= somaIds.size();
    r /= somaIds.size();

    SWCNode soma;
    soma.id = 1;
    soma.pid = -1;
    soma.type = 1;
    soma.x = x;
    soma.y = y;
    soma.z = z;
    soma.radius = r;

    std::map<int, SWCNode> newNodes;
    newNodes[1] = soma;

    int nextId = 2;
    std::unordered_map<int, int> idMap;

    for (const auto& [id, node] : inputNodes) {
        if (node.type == 1) continue;
        SWCNode newNode = node;
        newNode.id = nextId;
        idMap[id] = nextId;
        newNodes[nextId++] = newNode;
    }

    for (auto& [id, node] : newNodes) {
        if (id == 1) continue;
        if (inputNodes.at(node.pid).type == 1 || node.pid == 1) {
            node.pid = 1;
        } else {
            node.pid = idMap[node.pid];
        }
    }
	
	if (!isTopologicallySorted(newNodes)) {
		newNodes = topologicalSort(newNodes);
	}

    return newNodes;
}

/**
 * @brief Splits all edges in the neuron by inserting midpoint nodes
 * @param nodeSet The input map of SWC nodes to process
 * @return A new map of SWC nodes with midpoint nodes inserted on all edges
 * 
 * This method performs edge subdivision by inserting a new node at the midpoint
 * of every parent-child edge in the neuron morphology. The process involves:
 * 1. Creating a midpoint node for each parent-child relationship
 * 2. Positioning the midpoint at the average coordinates of parent and child
 * 3. Setting the midpoint radius as the average of parent and child radii
 * 4. Updating the topology so child nodes connect through their midpoints
 * 5. Ensuring topological sorting of the final result
 * 
 * This operation effectively doubles the resolution of the neuron morphology,
 * which is useful for:
 * - Mesh refinement operations
 * - Improved geometric accuracy in simulations
 * - Smoother interpolation between neuron segments
 * 
 * @note New node IDs are assigned sequentially starting from the highest existing ID + 1
 * @note Midpoint nodes inherit the type of their child node
 * @note Root nodes (pid == -1) are not affected by this operation
 * 
 * @see splitEdgesN() for performing multiple iterations of edge splitting
 * @see topologicalSort() for ensuring proper node ordering
 */
std::map<int, SWCNode> NeuronGraph::splitEdges(const std::map<int, SWCNode>& nodeSet) const {
    std::map<int, SWCNode> newNodes;

    // Efficiently get next available ID from the last (largest) key in the sorted map
    int nextId = nodeSet.rbegin()->first + 1;

    for (const auto& [id, node] : nodeSet) {
        newNodes[id] = node;

        if (node.pid != -1 && nodeSet.count(node.pid)) {
            const SWCNode& parentNode = nodeSet.at(node.pid);
            const SWCNode& childNode = node;

            // Create midpoint node
            SWCNode midNode;
            midNode.id = nextId;
            midNode.pid = parentNode.id;
            midNode.type = childNode.type;
            midNode.x = (parentNode.x + childNode.x) / 2.0;
            midNode.y = (parentNode.y + childNode.y) / 2.0;
            midNode.z = (parentNode.z + childNode.z) / 2.0;
            midNode.radius = (parentNode.radius + childNode.radius) / 2.0;

            newNodes[nextId] = midNode;

            // Update child node to have midpoint as parent
            SWCNode updatedChild = childNode;
            updatedChild.pid = nextId;
            newNodes[childNode.id] = updatedChild;

            ++nextId;
        }
    }

    // Ensure topological sort on the final new node set
    if (!isTopologicallySorted(newNodes)) {
        newNodes = topologicalSort(newNodes);
    }

    return newNodes;
}

/**
 * @brief Performs N iterations of edge splitting on the neuron morphology
 * @param nodeSet The input map of SWC nodes to process
 * @param N The number of edge splitting iterations to perform
 * @return A vector containing the node sets after each iteration of splitting
 * 
 * This method applies the splitEdges() operation N times in sequence, where each
 * iteration uses the result of the previous iteration as input. This creates
 * progressively finer subdivisions of the neuron morphology.
 * 
 * The returned vector contains N maps, where:
 * - splits[0] contains the result after 1 iteration
 * - splits[1] contains the result after 2 iterations
 * - splits[N-1] contains the result after N iterations
 * 
 * After N iterations, the number of nodes grows exponentially, with each
 * iteration approximately doubling the number of edges (and thus nodes).
 * 
 * Use cases:
 * - Progressive mesh refinement for adaptive simulations
 * - Creating multiple levels of detail for the same neuron
 * - Analyzing convergence properties of numerical methods
 * 
 * @note Each iteration significantly increases the number of nodes
 * @note Memory usage grows exponentially with N
 * @note Consider memory constraints when choosing large values of N
 * 
 * @see splitEdges() for single iteration edge splitting
 */
std::vector<std::map<int, SWCNode>> NeuronGraph::splitEdgesN(const std::map<int, SWCNode>& nodeSet, int N) const {
    std::vector<std::map<int, SWCNode>> splits;
    std::map<int, SWCNode> currentSet = nodeSet;

    for (int i = 0; i < N; ++i) {
        currentSet = this->splitEdges(currentSet);
        splits.push_back(currentSet);
    }

    return splits;
}

/**
 * @brief Performs topological sorting of neuron nodes to ensure proper parent-child ordering
 * @param nodeSet The input map of SWC nodes to sort
 * @return A new map of SWC nodes with reassigned IDs in topological order
 * 
 * This method implements Kahn's algorithm for topological sorting to ensure that
 * all parent nodes have smaller IDs than their children. The algorithm:
 * 1. Builds an adjacency list and calculates in-degrees for all nodes
 * 2. Starts with nodes having zero in-degree (root nodes)
 * 3. Processes nodes in breadth-first order, updating in-degrees
 * 4. Reassigns node IDs sequentially (1, 2, 3, ...) based on topological order
 * 5. Updates parent IDs to maintain correct relationships
 * 
 * Topological sorting is essential for:
 * - Ensuring proper tree traversal algorithms work correctly
 * - Maintaining consistency in neuron analysis operations
 * - Enabling efficient parent-to-child processing
 * - Supporting mesh generation algorithms that rely on ordered traversal
 * 
 * The resulting neuron will have:
 * - Root nodes (soma) with the smallest IDs
 * - All parent nodes having IDs smaller than their children
 * - Sequential ID numbering starting from 1
 * 
 * @note This method creates entirely new node IDs while preserving topology
 * @note The original node IDs are completely replaced
 * @note Parent-child relationships are preserved but with new ID mappings
 * 
 * @see isTopologicallySorted() to check if sorting is needed
 */
std::map<int, SWCNode> NeuronGraph::topologicalSort(const std::map<int, SWCNode>& nodeSet) const {
    std::map<int, std::vector<int>> adj;
    std::map<int, int> inDegree;
    std::vector<int> sortedOrder;
    std::map<int, SWCNode> sortedNodes;

    for (const auto& [id, node] : nodeSet) {
        if (node.pid != -1 && nodeSet.count(node.pid)) {
            adj[node.pid].push_back(id);
            inDegree[id]++;
        } else {
            inDegree[id]; // ensure all nodes are in inDegree
        }
    }

    std::queue<int> q;
    for (const auto& [id, deg] : inDegree)
        if (deg == 0) q.push(id);

    while (!q.empty()) {
        int id = q.front(); q.pop();
        sortedOrder.push_back(id);
        for (int child : adj[id])
            if (--inDegree[child] == 0) q.push(child);
    }

    std::map<int, int> oldToNewId;
    for (size_t i = 0; i < sortedOrder.size(); ++i)
        oldToNewId[sortedOrder[i]] = static_cast<int>(i) + 1;

    for (int oldId : sortedOrder) {
        SWCNode node = nodeSet.at(oldId);
        node.id = oldToNewId[oldId];
        node.pid = (node.pid == -1) ? -1 : oldToNewId[node.pid];
        sortedNodes[node.id] = node;
    }

    return sortedNodes;
}

/**
 * @brief Assigns a soma node to a neuron morphology that lacks one
 * @param nodeSet The input map of SWC nodes missing a soma
 * @return A new map of SWC nodes with a soma node assigned
 * 
 * This method addresses neuron morphologies that are missing a soma (cell body)
 * by converting the first root node found (pid == -1) into a soma node (type = 1).
 * 
 * The method follows this logic:
 * 1. First checks if a soma already exists using isSomaMissing()
 * 2. If soma is present, returns the input unchanged
 * 3. If soma is missing, searches for the first root node (pid == -1)
 * 4. Converts the root node's type to 1 (soma type)
 * 5. Prints a confirmation message with the assigned node ID
 * 
 * This operation is essential for:
 * - Ensuring every neuron has exactly one soma for proper analysis
 * - Correcting incomplete neuron reconstructions
 * - Preparing neuron data for mesh generation algorithms
 * - Maintaining consistency with neuron morphology standards
 * 
 * @note If no root node is found, prints a warning and returns input unchanged
 * @note Only the first root node encountered is converted to soma
 * @note The node's position and other properties remain unchanged
 * 
 * @warning If multiple root nodes exist, only the first one becomes the soma
 * 
 * @see isSomaMissing() to detect if soma assignment is needed
 * @see preprocess() which automatically calls this method when needed
 */
std::map<int, SWCNode> NeuronGraph::setSoma(const std::map<int, SWCNode>& nodeSet) const {
    if (!isSomaMissing(nodeSet)) {
        return nodeSet; // Soma already present
    }

    std::map<int, SWCNode> modified = nodeSet;

    for (auto& [id, node] : modified) {
        if (node.pid == -1) {
            node.type = 1; // Set as soma
            std::cout << "Assigned node ID " << id << " as soma (type 1).\n";
            return modified;
        }
    }

    std::cerr << "Warning: Soma is missing and no root node (pid = -1) found to assign.\n";
    return modified;
}
