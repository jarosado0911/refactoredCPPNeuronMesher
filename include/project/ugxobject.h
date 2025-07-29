#ifndef UGXOBJECT_H
#define UGXOBJECT_H

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

// 3D point representation
struct Coordinates {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

// Container for geometry data
struct UgxGeometry {
    std::map<int, Coordinates> points;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::array<int, 3>> faces;

    std::map<int, int> vertexSubsets;           // vertex index → subset ID
    std::map<int, int> edgeSubsets;             // edge index   → subset ID
    std::map<int, int> faceSubsets;             // face index   → subset ID
    std::map<int, std::string> subsetNames;     // subset ID    → name
};


// Main object to manage UGX geometry
class UgxObject {
private:
    UgxGeometry ugxg;

public:
    UgxObject() = default;
    UgxObject(const std::string& filename) {this->readUGX(filename);}
	UgxObject(const UgxGeometry& ugxinput) {this->ugxg = ugxinput;}

    void readUGX(const std::string& filename);

	// getter functions
    const UgxGeometry& getGeometry() const {return ugxg;}
	const std::map<int,Coordinates>& getPoints() const {return ugxg.points;}

	// printing functions
	void printCoordinates() const;
	void printEdges() const;
	void printFaces() const;
};

#endif // UGXOBJECT_H

