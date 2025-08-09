#include "neurongraph.h"
#include "ugxobject.h"
#include "utils.h"
#include <chrono>
#include <functional> // for std::hash
#include <sstream>

int main(int argc, char* argv[]){

    if (argc < 2 ) {
        std::cerr << "Usage: " << argv[0] << " <input.swc>\n";
        return 1;
    }

    std::cout << "Hello user!" << std::endl;

    // example of declaring the variable
    NeuronGraph graph;

    // example of reading a file
    graph.readFromFile(argv[1]);
    auto nodes = graph.removeSomaSegment();
    graph.setNodes(nodes);

    std::cout << "Neuron has " << graph.numberOfNodes() << " nodes\n";
    std::cout << "Neuron has " << graph.numberOfEdges() << " edges\n";

    bool resetIndex = true;
    auto trunks = graph.getTrunks(resetIndex);  
    
    std::string filename = argv[1];
    size_t pos = filename.rfind(".swc");  // Finds the last occurrence of ".swc"

    std::string base ="";
    if (pos != std::string::npos) {
        base = filename.substr(0, pos);
        base = std::filesystem::path(base).filename().string();
        std::cout << "Base name: " << base << std::endl;
    } else {
        std::cout << "No .swc extension found." << std::endl;
        
        // Generate a timestamp-based hash
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        std::size_t hash = std::hash<long long>{}(now);

        std::ostringstream oss;
        oss << "neuron_" << std::hex << hash;
        base = oss.str();
    }

    std::string execDir = getExecutableDir();
    std::string outputfolder = execDir + "/../output/" + base + "_trunks";
    checkFolder(outputfolder);

    for(auto& [id,trunk] : trunks){
        graph.writeToFile(trunk,outputfolder + "/trunk_"+std::to_string(id)+".swc");
    }

    std::string inputtrunk = outputfolder + "/trunk_4.swc";
    NeuronGraph atrunk;
    outputfolder = execDir + "/../output/main_pft_geometries";
    checkFolder(outputfolder);
    double delta = 0.75;

    UgxGeometry combined;
    UgxObject tempObj;

    for(auto& [id, path] : trunks){
        path = atrunk.cubicSplineResampleTrunk(path,delta);
        auto pft = atrunk.pftFromPath(path,16);
        combined = tempObj.addUGXGeometry(combined,pft.getGeometry()); 
        pft.writeUGX(outputfolder+"/pft_"+std::to_string(id)+".ugx");
    }

    //UgxObject g1(outputfolder+"/pft_"+std::to_string(1)+".ugx");
    //UgxObject g2(outputfolder+"/pft_"+std::to_string(2)+".ugx");
    //auto combined = g1.addUGXGeometry(g1.getGeometry(),g2.getGeometry());

    tempObj.setGeometry(combined);
    tempObj.writeUGX(outputfolder+"/ugxcombinedtest.ugx");

    /*
    UgxGeometry combinedBezierObj;
    UgxObject tempBezierObj;

    std::string inputBezier = execDir +  "/../output/test_bezier_subgraphs";
    const auto fileNames = listFilesInDirectory(inputBezier);
    for(auto f : fileNames)
    {
        NeuronGraph bezierPath(f);
        auto pft = bezierPath.pftFromPath(bezierPath.getNodes(), 16);
        combinedBezierObj = tempBezierObj.addUGXGeometry(combinedBezierObj,pft.getGeometry());    
    }

    outputfolder = execDir + "/../output";
    tempBezierObj.setGeometry(combinedBezierObj);
    tempBezierObj.writeUGX(outputfolder+"/branchBezierCombined.ugx");
*/
    return 0;
}
