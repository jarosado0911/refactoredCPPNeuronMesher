#include "neurongraph.h"

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

std::vector<std::map<int, SWCNode>> NeuronGraph::splitEdgesN(const std::map<int, SWCNode>& nodeSet, int N) const {
    std::vector<std::map<int, SWCNode>> splits;
    std::map<int, SWCNode> currentSet = nodeSet;

    for (int i = 0; i < N; ++i) {
        currentSet = this->splitEdges(currentSet);
        splits.push_back(currentSet);
    }

    return splits;
}

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
