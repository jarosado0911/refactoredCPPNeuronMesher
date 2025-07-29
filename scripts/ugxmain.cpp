#include "ugxobject.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.ugx>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    if (std::filesystem::path(filename).extension() != ".ugx") {
        std::cerr << "Invalid file type. Please provide a .ugx file.\n";
        return 1;
    }

    std::cout << "\nPass File name to UgxObject..." << std::endl;
    UgxObject obj(filename);
    obj.printCoordinates();
    obj.printEdges();
    obj.printFaces();

    std::cout << "\nRead a new file to previous UgxObject..." << std::endl;
    filename = "data/UGXMESHES/tetrahedron.ugx";
    obj.readUGX(filename);
    obj.printCoordinates();
    obj.printEdges();
    obj.printFaces();
    
    std::cout << "\nPass geometry from one UgxObject to another UgxObject..." << std::endl;
    UgxObject obj2(obj.getGeometry());
    obj2.printCoordinates();
    obj2.printEdges();
    obj2.printFaces();

    std::cout << "\n=== Looping over files in 'data/UGXMESHES/' ===\n" << std::endl;
    const std::filesystem::path meshDir = "data/UGXMESHES/";
    for (const auto& entry : std::filesystem::directory_iterator(meshDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".ugx") {
            std::cout << "\n>> Loading: " << entry.path() << std::endl;
            UgxObject objLoop(entry.path().string());
            objLoop.printCoordinates();
            objLoop.printEdges();
            objLoop.printFaces();
        }
    }

    return 0;
}
