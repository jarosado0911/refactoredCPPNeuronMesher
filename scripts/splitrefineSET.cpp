#include "neurongraph.h"
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

    // refine the geometry
    int N = 6;
    auto splitset = graph.splitEdgesN(N);
    
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
    std::string outputfolder = execDir + "/../output/" + base + "_refinements";
    checkFolder(outputfolder);

    int i = 1;
    for(auto& refinement : splitset){
        graph.writeToFile(refinement, outputfolder +"/refinement_"+std::to_string(i)+".swc");
        graph.writeToFileUGX(refinement, outputfolder +"/refinement_"+std::to_string(i)+".ugx");
        i++;
    }
    return 0;
}
