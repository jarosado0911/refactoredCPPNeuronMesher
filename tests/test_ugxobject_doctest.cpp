#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "externals/doctest.h"
#include "project/ugxobject.h"
#include "project/utils.h"
#include <filesystem>

TEST_CASE("UGXObject default constructor") {
    UgxObject u;
    CHECK(u.getPoints().size() == 0);
}

TEST_CASE("UGXObject read and print UGX") {
    std::string filepath = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(filepath);
    const auto& pts = u.getPoints();
    CHECK(pts.size() > 0);
}

TEST_CASE("UGXObject write and read") {
    std::string input = getExecutableDir() + "/../data/neuron.ugx";
    std::string output = getExecutableDir() + "/../output/test_output/test_ugx_write.ugx";
    UgxObject u(input);
    u.writeUGX(output);

    UgxObject u2(output);
    CHECK(u2.getPoints().size() == u.getPoints().size());
}

TEST_CASE("UGX Print Coordinates"){
    std::string input = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(input);
    u.printCoordinates();
    CHECK(true);
}

TEST_CASE("UGX Print Edges"){
    std::string input = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(input);
    u.printEdges();
    CHECK(true);
}

TEST_CASE("UGX Print Faces"){
    std::string input = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(input);
    u.printFaces();
    CHECK(true);
}