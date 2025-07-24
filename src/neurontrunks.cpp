#include "neurongraph.h"

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

// this method assumes the trunks that are extracted DO NOT have the node ids reset to start a 1
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

std::map<int, std::map<int, SWCNode>> NeuronGraph::allLinearSplineResampledTrunks(std::map<int, std::map<int, SWCNode>>& trunks, double& delta) const {
    std::map<int, std::map<int, SWCNode>> resampledTrunks;
    resampledTrunks.clear();

    for(auto& [id,trunk] : trunks){
            resampledTrunks[id] = linearSplineResampleTrunk(trunk, delta);
    }

    return resampledTrunks;
}

std::map<int, std::map<int, SWCNode>> NeuronGraph::allCubicSplineResampledTrunks(std::map<int, std::map<int, SWCNode>>& trunks, double& delta) const {
    std::map<int, std::map<int, SWCNode>> resampledTrunks;
    resampledTrunks.clear();

    for(auto& [id,trunk] : trunks){
            resampledTrunks[id] = cubicSplineResampleTrunk(trunk, delta);
    }

    return resampledTrunks;
}

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

// Cubic spline resampling (arc-length parameterized)
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