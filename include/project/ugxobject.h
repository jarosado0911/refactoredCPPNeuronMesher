#ifndef UGXOBJECT_H
#define UGXOBJECT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <queue>
#include <cmath>
#include <numeric>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <filesystem>

#include "neurongraph.h"

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
    std::map<int, double> radii;                // vertex index → radius
};


// Main object to manage UGX geometry
class UgxObject {
private:
    UgxGeometry ugxg;

public:
    UgxObject() = default;
    UgxObject(const std::string& filename) {this->readUGX(filename);}
	UgxObject(const UgxGeometry& ugxinput) {this->ugxg = ugxinput;}

	// read ugx file
    void readUGX(const std::string& filename);

	// write ugx file
	void writeUGX(const std::string& filename) const;

    // convert swc data (std::map<int,SWCNode>) to ugx geometry type
    const UgxGeometry convertToUGX(const std::map<int,SWCNode>& nodeSet);

	// getter functions
    const UgxGeometry& getGeometry() const {return ugxg;}
	const std::map<int,Coordinates>& getPoints() const {return ugxg.points;}
    const std::vector<std::pair<int, int>>& getEdges() const {return ugxg.edges;}
    const std::vector<std::array<int, 3>>& getFaces() const {return ugxg.faces;}

    // setter functions
    void setGeometry(const UgxGeometry& ugxinput){this->ugxg = ugxinput;};

	// printing functions
	void printCoordinates() const;
	void printEdges() const;
	void printFaces() const;
};

#endif // UGXOBJECT_H

