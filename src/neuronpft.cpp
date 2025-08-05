#include "ugxobject.h"
#include "neurongraph.h"
#include <vector>
#include <tuple>
#include <cmath>
#include <map>

struct Vec3 {
    double x, y, z;

    Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vec3 operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(double s) const { return {x / s, y / s, z / s}; }

    double norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 normalize() const {
        double n = this->norm();
        if (n < 1e-10) return {0, 0, 0};  // prevent division by zero
        return *this / n;
    }

    Vec3 cross(const Vec3& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }
};

inline Vec3 operator*(double s, const Vec3& v) { return v * s; }

struct Node {
    Vec3 pos;
    double radius;
    int type;
};

std::vector<std::tuple<Vec3, Vec3, Vec3>> computePTF(const std::vector<Node>& nodes) {
    std::vector<std::tuple<Vec3, Vec3, Vec3>> frames;
    if (nodes.size() < 2) return frames;

    Vec3 t0 = (nodes[1].pos - nodes[0].pos).normalize();
    Vec3 n0 = {0, 1, 0};
    if (std::abs(t0.cross(n0).norm()) < 1e-3) n0 = {1, 0, 0};
    Vec3 b0 = t0.cross(n0).normalize();
    n0 = b0.cross(t0).normalize();
    frames.emplace_back(t0, n0, b0);

    for (size_t i = 1; i < nodes.size(); ++i) {
        Vec3 t = (nodes[i].pos - nodes[i - 1].pos).normalize();
        Vec3 b_prev = std::get<2>(frames.back());
        Vec3 n = b_prev.cross(t).normalize();
        Vec3 b = t.cross(n).normalize();
        frames.emplace_back(t, n, b);
    }
    return frames;
}

// Main method
UgxObject NeuronGraph::pftFromPath(const std::map<int, SWCNode>& path, int segments) {
    std::vector<Node> nodes;
    for (const auto& [_, swc] : path) {
        nodes.push_back({{swc.x, swc.y, swc.z}, swc.radius, swc.type});
    }

    auto frames = computePTF(nodes);
    UgxGeometry geom;
    int vid = 0;

    for (size_t i = 0; i < nodes.size(); ++i) {
        const auto& [T, N, B] = frames[i];
        const Vec3& P = nodes[i].pos;
        double r = nodes[i].radius;

        for (int j = 0; j < segments; ++j) {
            double theta = 2.0 * M_PI * j / segments;
            Vec3 circ = std::cos(theta) * N + std::sin(theta) * B;
            Vec3 v = P + circ * r;

            geom.points[vid] = {v.x, v.y, v.z};
            geom.radii[vid] = r;
            geom.vertexSubsets[vid] = nodes[i].type;
            ++vid;
        }
    }

    int numRings = nodes.size();
    for (int i = 0; i < numRings - 1; ++i) {
        for (int j = 0; j < segments; ++j) {
            int a = i * segments + j;
            int b = i * segments + (j + 1) % segments;
            int c = (i + 1) * segments + j;
            int d = (i + 1) * segments + (j + 1) % segments;

            geom.edges.push_back({a, c});
            geom.edges.push_back({a, b});
            geom.edges.push_back({c, d});

            geom.faces.push_back({a, b, c});
            geom.faces.push_back({b, d, c});

            geom.edgeSubsets[geom.edges.size() - 3] = nodes[i].type;
            geom.edgeSubsets[geom.edges.size() - 2] = nodes[i].type;
            geom.edgeSubsets[geom.edges.size() - 1] = nodes[i].type;

            geom.faceSubsets[geom.faces.size() - 2] = nodes[i].type;
            geom.faceSubsets[geom.faces.size() - 1] = nodes[i].type;
        }
    }

    // Assign subset names
    std::map<int, std::string> subsetTypeNames = {
        {1, "Soma"},
        {2, "Axon"},
        {3, "Dendrite"},
        {4, "ApicalDendrite"},
        {5, "ForkPoint"},
        {6, "EndPoint"},
        {7, "Custom"}
    };

    std::set<int> usedTypes;
    for (const auto& [_, node] : path) {
        usedTypes.insert(node.type);
    }

    for (int typeId : usedTypes) {
        if (subsetTypeNames.count(typeId))
            geom.subsetNames[typeId] = subsetTypeNames[typeId];
        else
            geom.subsetNames[typeId] = "UnknownType_" + std::to_string(typeId);
    }

    UgxObject obj;
    obj.setGeometry(geom);

    return obj;
}