#include "neurongraph.h"

/**
 * @brief Builds an adjacency list representing the connectivity between nodes in a neuron morphology
 * @param nodeSet Map of SWC nodes where key is node ID and value is the SWCNode object
 * @return std::map<int, std::vector<int>> where each key is a node ID and value is a vector of its neighbor node IDs
 * 
 * This method constructs an undirected graph representation of the neuron morphology by creating
 * neighbor relationships between connected nodes. For each node in the input set, it establishes
 * bidirectional connections between the node and its parent (if the parent exists in the nodeSet).
 * 
 * The resulting neighbor map allows efficient traversal of the neuron's topology in any direction,
 * which is essential for various analysis and processing tasks such as trunk extraction and
 * topological sorting.
 * 
 * @note Only nodes present in the input nodeSet are included in the neighbor map
 * @note The method creates bidirectional edges (A→B and B→A) for each parent-child relationship
 * @see getTrunks() for the primary use of this method
 * @see topologicalSort() for another application of the neighbor map
 */
std::map<int, std::vector<int>> NeuronGraph::getNeighborMap(const std::map<int, SWCNode>& nodeSet) const {
    std::map<int, std::vector<int>> neighbors;

    for (const auto& [id, node] : nodeSet) {
        if (node.pid != -1 && nodes.count(node.pid)) {
            neighbors[id].push_back(node.pid);
            neighbors[node.pid].push_back(id);
        }
    }
    return neighbors;
}

/**
 * @brief Extracts trunk segments from a neuron morphology
 * @param nodeSet Map of SWC nodes where key is node ID and value is the SWCNode object
 * @param resetIndex If true, renumbers node IDs sequentially within each trunk (default: false)
 * @return std::map<int, std::map<int, SWCNode>> where each key is a trunk ID and value is a map of node IDs to SWCNodes in that trunk
 * 
 * This method identifies and extracts linear segments (trunks) from a neuron morphology. A trunk is defined as a path between:
 * - Two branch points (nodes with degree > 2)
 * - A branch point and an endpoint (node with degree 1)
 * - The root node and the first branch point
 *
 * The algorithm works as follows:
 * 1. Builds a neighbor map to represent the neuron's connectivity
 * 2. Identifies branch points (nodes with >2 neighbors)
 * 3. For each branch point, traverses to connected nodes to extract linear segments
 * 4. Uses canonical representation to avoid duplicate trunks
 * 5. Optionally renumbers node IDs sequentially within each trunk
 *
 * The method handles both directed and undirected traversal of the neuron morphology.
 *
 * @note When resetIndex is true, node IDs within each trunk will be renumbered sequentially starting from 1,
 *       with parent IDs adjusted accordingly. The first node in each trunk will have pid = -1.
 * @note The method uses a canonical representation to ensure each trunk is only included once,
 *       regardless of traversal direction.
 * @see getNeighborMap() for the graph representation used internally
 * @see getTrunkParentMap() for establishing relationships between trunks
 */
std::map<int, std::map<int, SWCNode>> NeuronGraph::getTrunks(const std::map<int, SWCNode>& nodeSet, bool resetIndex) const {
    std::map<int, std::map<int, SWCNode>> trunkNodeSets;
    std::map<int, std::vector<int>> neighborMap = getNeighborMap(nodeSet);
    std::set<int> visited;
    std::set<std::string> seen;
    int trunkId = 0;

    auto canonicalKey = [](const std::vector<int>& v) {
        std::vector<int> fwd = v, rev = v;
        std::reverse(rev.begin(), rev.end());
        const auto& best = (fwd < rev) ? fwd : rev;
        std::ostringstream oss;
        for (int n : best) oss << n << ",";
        return oss.str();
    };

    for (const auto& [id, neighbors] : neighborMap) {
        if (neighbors.size() > 2) {
            for (int nbr : neighbors) {
                if (visited.count(nbr)) continue;

                std::vector<int> trunk = {id};
                int prev = id, curr = nbr;

                while (neighborMap[curr].size() == 2 && !visited.count(curr)) {
                    trunk.push_back(curr);
                    visited.insert(curr);

                    const auto& nexts = neighborMap[curr];
                    curr = (nexts[0] == prev) ? nexts[1] : nexts[0];
                    prev = trunk.back();
                }

                trunk.push_back(curr);
                std::string key = canonicalKey(trunk);
                if (seen.count(key)) continue;

                seen.insert(key);

                std::map<int, SWCNode> newNodeSet;
                if (resetIndex) {
                    int localId = 1;
                    for (size_t i = 0; i < trunk.size(); ++i) {
                        SWCNode n = nodeSet.at(trunk[i]);
                        n.id = localId;
                        n.pid = (i == 0) ? -1 : localId - 1;
                        newNodeSet[n.id] = n;
                        ++localId;
                    }
                } else {
                    for (int nid : trunk) {
                        newNodeSet[nid] = nodeSet.at(nid);
                    }
                }
                trunkNodeSets[trunkId++] = std::move(newNodeSet);
            }
        }
    }

    return trunkNodeSets;
}

/**
 * @brief Determines the parent-child relationships between trunk segments in a neuron morphology
 * @param nodeSet Original map of all SWC nodes in the neuron
 * @param trunkNodeSets Map of trunk segments where each key is a trunk ID and value is a map of node IDs to SWCNodes
 * @return std::map<int, int> Map where each key is a trunk ID and value is its parent trunk ID (-1 for root trunks)
 * 
 * This method analyzes the connectivity between trunk segments to establish a hierarchical
 * parent-child relationship between them. It determines which trunk connects to which by examining
 * the parent-child relationships of the nodes at the connection points between trunks.
 *
 * The algorithm works in two main phases:
 * 1. Builds a reverse lookup (node ID → trunk ID) to quickly find which trunk a node belongs to
 * 2. For each trunk, examines the parent of its first node to determine its parent trunk
 *
 * Key assumptions:
 * - The first node in each trunk's node map is the rootward (proximal) end of the trunk
 * - Trunks are topologically sorted with parent nodes appearing before their children
 * - The input trunks have already been extracted and may have been processed (e.g., resampled)
 *
 * @note A return value of -1 indicates a root trunk (no parent)
 * @note If a trunk's first node has no parent or its parent is not in any trunk, it's considered a root trunk
 * @see getTrunks() for how trunks are initially extracted
 * @see assembleTrunks() for how this parent information is used in reconstruction
 */
std::map<int, int> NeuronGraph::getTrunkParentMap(const std::map<int, SWCNode>& nodeSet,
                                                const std::map<int, std::map<int, SWCNode>>& trunkNodeSets) const {
    std::map<int, int> trunkParentMap;
    std::map<int, int> nodeToTrunk;

    // Build reverse lookup: node ID → trunk ID
    for (const auto& [trunkId, trunk] : trunkNodeSets) {
        for (const auto& [nodeId, node] : trunk) {
            nodeToTrunk[nodeId] = trunkId;
        }
    }

    // Now, determine the parent trunk for each trunk
    for (const auto& [trunkId, trunk] : trunkNodeSets) {
        if (trunk.empty()) continue;

        // Get the first node in the trunk (assumed topologically sorted)
        const SWCNode& firstNode = trunk.begin()->second;
        int parentId = nodeSet.at(firstNode.id).pid;

        // If it has a valid parent in nodeSet, find the trunk it came from
        if (parentId != -1 && nodeToTrunk.count(parentId)) {
            int parentTrunkId = nodeToTrunk[parentId];
            trunkParentMap[trunkId] = parentTrunkId;
        } else {
            trunkParentMap[trunkId] = -1;  // No parent trunk (i.e., root trunk)
        }
    }

    return trunkParentMap;
}

/**
 * @brief Combines multiple trunk segments into a single neuron representation with sequential node IDs
 * @param trunkNodeSets Map of trunk segments where each key is a trunk ID and value is a map of node IDs to SWCNodes
 * @return std::map<int, SWCNode> A single map of SWC nodes with sequential IDs representing the assembled neuron
 * 
 * This method takes multiple trunk segments and combines them into a single neuron representation
 * while ensuring all node IDs are unique and sequential. It handles the remapping of parent-child
 * relationships to maintain the neuron's topological structure.
 *
 * Key features:
 * - Renumbers all node IDs sequentially starting from 1
 * - Maintains parent-child relationships between nodes
 * - Handles duplicate nodes across different trunks (uses first occurrence)
 * - Ensures the root node has pid = -1
 *
 * The method performs two main passes:
 * 1. First pass: Creates a mapping from old node IDs to new sequential IDs
 * 2. Second pass: Updates all parent IDs using the new mapping
 *
 * @note This method assumes that node IDs within each trunk have NOT been reset to start at 1.
 *       If the trunks have already been renumbered, use the other overload that takes a trunkParentMap.
 * @note The resulting node IDs will be sequential and unique across all trunks.
 * @see assembleTrunks(const std::map<int, std::map<int, SWCNode>>&, const std::map<int,int>&)
 *      for the version that handles parent-child relationships between trunks
 */
std::map<int, SWCNode> NeuronGraph::assembleTrunks(const std::map<int, std::map<int, SWCNode>>& trunkNodeSets) const {
    std::map<int, SWCNode> newNodes;
    std::map<int, int> remap;
    int newId = 1;

    for (const auto& [tid, trunk] : trunkNodeSets) {
        for (const auto& [oldId, node] : trunk) {
            if (remap.count(oldId)) continue;

            SWCNode newNode = node;
            newNode.id = newId;
            remap[oldId] = newId;
            newNodes[newId] = newNode;
            ++newId;
        }
    }

    for (auto& [id, node] : newNodes) {
        if (node.pid != -1 && remap.count(node.pid)) {
            node.pid = remap[node.pid];
        } else {
            node.pid = -1;
        }
    }

    return newNodes;
}

/**
 * @brief Applies linear spline resampling to all trunk segments in a neuron morphology
 * @param trunks Map of trunk segments where each key is a trunk ID and value is a map of node IDs to SWCNodes
 * @param delta Target distance between consecutive nodes in the resampled trunks (in microns)
 * @return std::map<int, std::map<int, SWCNode>> Map of resampled trunks with the same structure as input
 * 
 * This method processes each trunk segment in the input map and applies linear spline resampling
 * to achieve a more uniform node distribution. The resampling is performed independently on each trunk.
 *
 * Key features:
 * - Preserves the start and end points of each trunk exactly
 * - Inserts new nodes to achieve approximately 'delta' spacing between points
 * - Maintains the original topology and branching structure
 * - Handles each trunk segment independently
 *
 * The resampling process involves:
 * 1. Calculating the total length of each trunk
 * 2. Determining the number of segments needed based on 'delta'
 * 3. Linearly interpolating new points along each segment
 *
 * @note The actual spacing may vary slightly to ensure the start and end points are preserved exactly
 * @note This method is typically used as a preprocessing step before mesh generation
 * @see linearSplineResampleTrunk() for the single-trunk implementation
 * @see allCubicSplineResampledTrunks() for a smoother but more computationally intensive alternative
 */
std::map<int, std::map<int, SWCNode>> NeuronGraph::allLinearSplineResampledTrunks(std::map<int, std::map<int, SWCNode>>& trunks, double& delta) const {
    std::map<int, std::map<int, SWCNode>> resampledTrunks;
    resampledTrunks.clear();

    for(auto& [id,trunk] : trunks){
            resampledTrunks[id] = linearSplineResampleTrunk(trunk, delta);
    }

    return resampledTrunks;
}

/**
 * @brief Applies cubic spline resampling to all trunk segments in a neuron morphology
 * @param trunks Map of trunk segments where each key is a trunk ID and value is a map of node IDs to SWCNodes
 * @param delta Target distance between consecutive nodes in the resampled trunks (in microns)
 * @return std::map<int, std::map<int, SWCNode>> Map of resampled trunks with the same structure as input
 * 
 * This method processes each trunk segment in the input map and applies cubic spline resampling
 * to achieve a smooth and uniform node distribution. The resampling is performed independently on each trunk.
 *
 * Key features:
 * - Fits a smooth cubic spline through the original nodes of each trunk
 * - Samples points at approximately 'delta' intervals along the spline
 * - Preserves the start and end points of each trunk exactly
 * - Maintains the original topology while improving smoothness
 * - Handles each trunk segment independently
 *
 * The resampling process involves:
 * 1. Fitting natural cubic splines to the x, y, and z coordinates separately
 * 2. Parameterizing the spline by arc length
 * 3. Sampling new points at approximately 'delta' intervals along the curve
 *
 * @note This method produces smoother results than linear interpolation but is more computationally intensive
 * @note The actual spacing may vary slightly to ensure the curve is sampled properly
 * @see cubicSplineResampleTrunk() for the single-trunk implementation
 * @see allLinearSplineResampledTrunks() for a faster but less smooth alternative
 */
std::map<int, std::map<int, SWCNode>> NeuronGraph::allCubicSplineResampledTrunks(std::map<int, std::map<int, SWCNode>>& trunks, double& delta) const {
    std::map<int, std::map<int, SWCNode>> resampledTrunks;
    resampledTrunks.clear();

    for(auto& [id,trunk] : trunks){
            resampledTrunks[id] = cubicSplineResampleTrunk(trunk, delta);
    }

    return resampledTrunks;
}

/**
 * @brief Resamples a single trunk segment using linear interpolation
 * @param trunk Map of node IDs to SWCNode objects representing the trunk to be resampled
 * @param delta Target distance between consecutive nodes in the resampled trunk (in microns)
 * @return std::map<int, SWCNode> A new map of resampled nodes with sequential IDs
 * 
 * This method performs linear resampling of a neuron trunk to achieve a more uniform
 * distribution of nodes along its length. It creates a piecewise linear approximation
 * of the original trunk with nodes spaced approximately 'delta' units apart.
 *
 * Key features:
 * - Preserves the first and last nodes of the original trunk exactly
 * - Uses linear interpolation between existing nodes
 * - Maintains the dominant node type throughout the resampled trunk
 * - Adjusts node IDs and parent IDs to maintain a valid tree structure
 * - Handles 3D coordinates and radii
 *
 * The algorithm works as follows:
 * 1. Converts the input trunk to an ordered vector of nodes
 * 2. Determines the dominant node type in the trunk
 * 3. Calculates the total arc length of the trunk
 * 4. Determines the number of segments needed based on 'delta'
 * 5. Performs linear interpolation between existing nodes to create new points
 * 6. Ensures the first and last nodes match the original exactly
 *
 * @note The actual spacing may vary slightly to ensure the first and last nodes are preserved exactly
 * @note If the input trunk has fewer than 2 nodes, returns an empty map
 * @note The minimum number of points in the resampled trunk is 4 to ensure reasonable shape representation
 * @see allLinearSplineResampledTrunks() for resampling multiple trunks
 * @see cubicSplineResampleTrunk() for a smoother but more computationally intensive alternative
 */
std::map<int, SWCNode> NeuronGraph::linearSplineResampleTrunk(const std::map<int, SWCNode>& trunk, double& delta) const {
    std::map<int, SWCNode> newNodes;
    std::vector<SWCNode> sampledNodes;
    std::map<int, int> typeCount;

    // Convert trunk map to ordered vector of nodes
    for (const auto& [id, node] : trunk) {
        sampledNodes.push_back(node);
        typeCount[node.type]++;
    }

    if (sampledNodes.size() < 2) return newNodes;

    // Determine the dominant type
    int dominantType = sampledNodes[0].type;
    int maxCount = 0;
    for (const auto& [type, count] : typeCount) {
        if (count > maxCount) {
            dominantType = type;
            maxCount = count;
        }
    }

    // Compute total arc length
    double totalLength = 0.0;
    for (size_t i = 1; i < sampledNodes.size(); ++i) {
        double dx = sampledNodes[i].x - sampledNodes[i - 1].x;
        double dy = sampledNodes[i].y - sampledNodes[i - 1].y;
        double dz = sampledNodes[i].z - sampledNodes[i - 1].z;
        totalLength += std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    int N =  static_cast<int>(std::round(totalLength / delta));
    if (N <= 3) N = 4;

    int newId = 1;
    for (int j = 0; j < N; ++j) {
        double t = static_cast<double>(j) / (N - 1);
        size_t seg = static_cast<size_t>(t * (sampledNodes.size() - 1));
        size_t next = std::min(seg + 1, sampledNodes.size() - 1);
        double alpha = t * (sampledNodes.size() - 1) - seg;

        SWCNode interp;

        if (j == 0 || j == N - 1) {
            interp = sampledNodes[j == 0 ? 0 : sampledNodes.size() - 1];
            interp.id = newId;
            interp.pid = (j == 0) ? -1 : newId - 1;
        } else {
            auto lerp = [](double a, double b, double alpha) {return (1 - alpha) * a + alpha * b;};

            interp.id = newId;
            interp.pid = newId - 1;
            interp.type = dominantType;
            interp.x = lerp(sampledNodes[seg].x, sampledNodes[next].x, alpha);
            interp.y = lerp(sampledNodes[seg].y, sampledNodes[next].y, alpha);
            interp.z = lerp(sampledNodes[seg].z, sampledNodes[next].z, alpha);
            interp.radius = std::abs(lerp(sampledNodes[seg].radius, sampledNodes[next].radius, alpha));
        }

        newNodes[newId++] = interp;
    }

    return newNodes;
}

/**
 * @brief Resamples a single trunk segment using cubic spline interpolation
 * @param trunk Map of node IDs to SWCNode objects representing the trunk to be resampled
 * @param delta Target distance between consecutive nodes in the resampled trunk (in microns)
 * @return std::map<int, SWCNode> A new map of resampled nodes with sequential IDs
 * 
 * This method performs cubic spline resampling of a neuron trunk to achieve a smooth
 * and uniform distribution of nodes along its length. It creates a smooth curve that
 * passes through all original nodes and samples new points along this curve.
 *
 * Key features:
 * - Preserves the first and last nodes of the original trunk exactly
 * - Fits natural cubic splines to x, y, and z coordinates separately
 * - Parameterizes the spline by arc length for uniform sampling
 * - Maintains the dominant node type throughout the resampled trunk
 * - Handles 3D coordinates and radii with appropriate interpolation
 * - Adjusts node IDs and parent IDs to maintain a valid tree structure
 *
 * The algorithm works as follows:
 * 1. Converts the input trunk to an ordered vector of nodes
 * 2. Determines the dominant node type in the trunk
 * 3. Fits natural cubic splines to x, y, and z coordinates
 * 4. Parameterizes the spline by arc length
 * 5. Samples new points at approximately 'delta' intervals along the curve
 * 6. Ensures the first and last nodes match the original exactly
 *
 * @note This method produces smoother results than linear interpolation but is more computationally intensive
 * @note The actual spacing may vary slightly to ensure the curve is sampled properly
 * @note If the input trunk has fewer than 4 nodes, falls back to linear interpolation
 * @see allCubicSplineResampledTrunks() for resampling multiple trunks
 * @see linearSplineResampleTrunk() for a faster but less smooth alternative
 */
std::map<int, SWCNode> NeuronGraph::cubicSplineResampleTrunk(const std::map<int, SWCNode>& trunk, double& delta) const {
    std::vector<SWCNode> sampledNodes;
    std::map<int, SWCNode> newNodes;
    std::map<int, int> typeCount;

    // Convert trunk map to ordered vector of nodes
    for (const auto& [id, node] : trunk) {
        sampledNodes.push_back(node);
        typeCount[node.type]++;
    }

    if (sampledNodes.size() < 2) return newNodes;

    int dominantType = sampledNodes[0].type;
    int maxCount = 0;
    for (const auto& [type, count] : typeCount) {
        if (count > maxCount) {
            dominantType = type;
            maxCount = count;
        }
    }

    std::vector<double> arcLength = {0.0};
    for (size_t i = 1; i < sampledNodes.size(); ++i) {
        double dx = sampledNodes[i].x - sampledNodes[i - 1].x;
        double dy = sampledNodes[i].y - sampledNodes[i - 1].y;
        double dz = sampledNodes[i].z - sampledNodes[i - 1].z;
        arcLength.push_back(arcLength.back() + std::sqrt(dx * dx + dy * dy + dz * dz));
    }
    double totalLength = arcLength.back();

    int N =  static_cast<int>(std::round(totalLength / delta));
    if (N <= 3) N = 4;

    std::vector<double> ts(N);
    for (int i = 0; i < N; ++i) ts[i] = i * totalLength / (N - 1);

    auto cubicSpline = [](const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& ts) {
        int n = x.size();
        std::vector<double> h(n - 1), alpha(n - 1), l(n), mu(n), z(n);
        std::vector<double> b(n - 1), c(n), d(n - 1);

        for (int i = 0; i < n - 1; ++i)
            h[i] = x[i + 1] - x[i];

        for (int i = 1; i < n - 1; ++i)
            alpha[i] = (3.0 / h[i]) * (y[i + 1] - y[i]) - (3.0 / h[i - 1]) * (y[i] - y[i - 1]);

        l[0] = 1; mu[0] = z[0] = 0;
        for (int i = 1; i < n - 1; ++i) {
            l[i] = 2 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
            mu[i] = h[i] / l[i];
            z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
        }
        l[n - 1] = 1; z[n - 1] = c[n - 1] = 0;

        for (int j = n - 2; j >= 0; --j) {
            c[j] = z[j] - mu[j] * c[j + 1];
            b[j] = (y[j + 1] - y[j]) / h[j] - h[j] * (c[j + 1] + 2 * c[j]) / 3.0;
            d[j] = (c[j + 1] - c[j]) / (3.0 * h[j]);
        }

        std::vector<double> result;
        for (double xq : ts) {
            int i = 0;
            while (i < n - 2 && xq > x[i + 1]) ++i;
            double dx = xq - x[i];
            result.push_back(y[i] + b[i] * dx + c[i] * dx * dx + d[i] * dx * dx * dx);
        }
        return result;
    };

    std::vector<double> xs, ys, zs, rs;
    for (const auto& p : sampledNodes) {
        xs.push_back(p.x);
        ys.push_back(p.y);
        zs.push_back(p.z);
        rs.push_back(p.radius);
    }

    auto xNew = cubicSpline(arcLength, xs, ts);
    auto yNew = cubicSpline(arcLength, ys, ts);
    auto zNew = cubicSpline(arcLength, zs, ts);
    auto rNew = cubicSpline(arcLength, rs, ts);

    //double avgRadius = std::accumulate(rs.begin(), rs.end(), 0.0) / rs.size();
    double minRadius = *std::min_element(rs.begin(), rs.end());
    double clampRadius = 1.05 * minRadius;

    int newId = 1;
    for (int i = 0; i < N; ++i) {
        SWCNode node;
        if (i == 0 || i == N - 1) {
            node = sampledNodes[i == 0 ? 0 : sampledNodes.size() - 1];
            node.id = newId;
            node.pid = (i == 0) ? -1 : newId - 1;
        } else {
            node.id = newId;
            node.pid = newId - 1;
            node.type = dominantType;
            node.x = xNew[i];
            node.y = yNew[i];
            node.z = zNew[i];
            node.radius = std::max(std::abs(rNew[i]), clampRadius);
        }
        newNodes[newId++] = node;
    }

    return newNodes;
}

std::map<int, SWCNode> NeuronGraph::assembleTrunks(const std::map<int, std::map<int, SWCNode>>& resampledTrunks,
									               const std::map<int,int>& trunkParentMap){
    std::map<int, SWCNode> finalNodes;
    // a new variable that stores (trunkid,startid,endid) startid and end id are the globalId numbers
    std::map<int,std::vector<int>> trunkEnds;
    std::vector<int> doneTrunks;
    SWCNode somaNode;                                    

    // Find the trunks that have soma point, and make first node of finalNodes
    for(auto& [trunkId, trunk] : resampledTrunks){
        for(auto& [ _ , node] : trunk){
            if(node.type == 1){
                doneTrunks.push_back(trunkId);
                somaNode = node;
                break;
            }
        }
    }

    // initialize soma node
    somaNode.id = 1; somaNode.pid = -1;
    // set the first node in final nodes
    finalNodes[somaNode.id] = somaNode;

    int globalId = 1;
    //Now we iterate the doneTrunks and add all trunks that are connected to soma
    for(auto& trunkId : doneTrunks){
        trunkEnds[trunkId].push_back(1); // this captures the new id of the endpoint (startId)
        for(auto& [_ , node]: resampledTrunks.at(trunkId) ){
            if (node.pid != -1){
                auto newNode = node;
                newNode.id = globalId+1;
                if(node.id == 2) newNode.pid = 1;
                if(node.id>2) newNode.pid = globalId;

                finalNodes[newNode.id]=newNode;
                globalId = globalId + 1;
            }
        }
        trunkEnds[trunkId].push_back(globalId); // this captures the new id of the endpoint (endId)
    }

    for(auto& [trunkId, trunk]  : resampledTrunks){
        
        trunkEnds[trunkId].push_back(globalId); // this captures the new id of the endpoint (startId)
        if (std::find(doneTrunks.begin(), doneTrunks.end(), trunkId) != doneTrunks.end()) {
            continue;
        }
        else {
            for(auto& [_ , node] : resampledTrunks.at(trunkId)) {
                if (node.pid != -1){
                    auto newNode = node;
                    newNode.id = globalId+1;
                
                    if(node.id >2) newNode.pid = globalId;
                    else newNode.pid = -1;
                    finalNodes[newNode.id]=newNode;
                    globalId = globalId + 1;
                }
            }
        }
        trunkEnds[trunkId].push_back(globalId); // this captures the new id of the endpoint (endId)
    }

    // Lambda to compute Euclidean distance between nodes
    auto distance = [](const SWCNode& a, const SWCNode& b) -> double {
        double dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    };

    // now go back and connect the branches correctly
    for(auto& [trunkId, _] : resampledTrunks){
        if (std::find(doneTrunks.begin(), doneTrunks.end(), trunkId) != doneTrunks.end()) {
            continue;
        }
       
        int parentTrunkId = trunkParentMap.at(trunkId);
        int parentStartId = trunkEnds[parentTrunkId][0];
        int parentEndId   = trunkEnds[parentTrunkId][1];
        int childStartId  = trunkEnds[trunkId][0];

        const SWCNode& parentStart = finalNodes[parentStartId];
        const SWCNode& parentEnd   = finalNodes[parentEndId];
        const SWCNode& childStart  = finalNodes[childStartId+1];

        finalNodes[childStartId+1].pid = (distance(childStart, parentEnd) < distance(childStart, parentStart))
            ? parentEndId
            : parentStartId;

    }

    return finalNodes;
}

/**
 * @brief Generates multiple levels of mesh refinements for a neuron morphology
 * @param nodeSet Map of SWC nodes representing the input neuron morphology
 * @param delta Initial target spacing between nodes (in microns). This value is halved in each refinement level.
 * @param N Number of refinement levels to generate
 * @param method Interpolation method to use ("linear" or "cubic")
 * @return std::map<int, std::map<int, SWCNode>> Map where each key is the refinement level (0 to N-1)
 *         and the value is the resampled neuron at that refinement level
 * 
 * This method creates a series of increasingly refined versions of the input neuron morphology.
 * Each refinement level approximately halves the spacing between nodes, providing a multi-resolution
 * representation of the neuron that can be used for progressive rendering or adaptive meshing.
 *
 * Key features:
 * - Supports both linear and cubic spline interpolation methods
 * - Progressively refines the mesh by halving 'delta' at each level
 * - Maintains topological relationships between nodes
 * - Preserves branch points and overall neuron structure
 * - Returns all refinement levels for comparison or progressive loading
 *
 * The refinement process works as follows:
 * 1. Extracts trunk segments from the input neuron
 * 2. Determines parent-child relationships between trunks
 * 3. For each refinement level i (0 to N-1):
 *    a. Resamples all trunks using the specified method with spacing delta/(2^i)
 *    b. Reassembles the neuron from the resampled trunks
 *    c. Stores the result in the output map
 *
 * @note The input 'delta' parameter will be modified during execution (halved at each level)
 * @note If an unsupported method is provided, falls back to linear interpolation
 * @note The first refinement level (i=0) uses the initial delta value
 * @see allLinearSplineResampledTrunks() for the linear interpolation implementation
 * @see allCubicSplineResampledTrunks() for the cubic spline implementation
 */
std::map<int, std::map<int,SWCNode>> NeuronGraph::generateRefinements(const std::map<int,SWCNode>& nodeSet, 
                                                                     double& delta, int& N, std::string& method){
    bool resetIndex = false;
    auto trunks = getTrunks(nodeSet,resetIndex);
    auto trunkParentMap = getTrunkParentMap(nodeSet,trunks);
    std::map<int,std::map<int,SWCNode>> refinements;
    std::map<int,std::map<int, SWCNode>> resampledTrunks;

    for(int i=0; i < N; ++i){
        if (method == "cubic"){
            resampledTrunks = allCubicSplineResampledTrunks(trunks,delta);
        } else if (method == "linear") {
            resampledTrunks = allLinearSplineResampledTrunks(trunks,delta);
        } else {
            resampledTrunks = allLinearSplineResampledTrunks(trunks,delta);
        }
        auto newNodes = assembleTrunks(resampledTrunks,trunkParentMap);
        refinements[i]=newNodes;
        delta = delta/2;
    }

    return refinements;
}