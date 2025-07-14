#include "neurongraph.h"
#include "utils.h"

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
    std::cout << "Neuron has " << graph.numberOfNodes() << " nodes\n";
    std::cout << "Neuron has " << graph.numberOfEdges() << " edges\n";

    // example of writing to a file
    std::string execDir = getExecutableDir();
    std::string outputfolder = execDir + "/../output/main_output/";
    graph.writeToFile(graph.getNodes(),outputfolder+"examplewrite.swc");    

    return 0;
}
