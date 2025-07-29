#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "externals/doctest.h"
#include "project/ugxobject.h"
#include "project/utils.h"
#include <filesystem>

TEST_CASE("UGXObject default constructor") {
    UgxObject u;
    CHECK(u.getPoints().size() == 0);
}

TEST_CASE("UGXObject constructor overload 1"){
    std::string filepath = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(filepath);
    const auto& pts = u.getPoints();
    CHECK(pts.size() > 0);
}

TEST_CASE("UGXObject constructor overload 2"){
    std::string filepath = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(filepath);
    const auto geom = u.getGeometry();
    UgxObject u2(geom);
    CHECK(u.getPoints().size() == u2.getPoints().size());
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

TEST_CASE("UGXObject loads a UGX file correctly") {
    std::string input = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(input);

    const auto& geom = u.getGeometry();
    CHECK(!geom.points.empty());
    CHECK(geom.points.size() > 0);
}

TEST_CASE("UGXObject writes a UGX file correctly") {
    std::string output = getExecutableDir() + "/../output/test_output/";
    UgxGeometry geom;
    geom.points[0] = {0, 0, 0};
    geom.points[1] = {1, 0, 0};
    geom.edges.emplace_back(0, 1);
    geom.subsetNames[0] = "test";

    UgxObject u(geom);
    u.writeUGX(output+"test_output.ugx");

    // Read back
    UgxObject u2(output+"test_output.ugx");
    CHECK(u2.getGeometry().points.size() == 2);
}

TEST_CASE("UGXObject printing methods do not crash") {
    UgxGeometry geom;
    geom.points[0] = {0.1, 0.2, 0.3};
    geom.points[1] = {1.0, 1.2, 1.3};
    geom.edges.emplace_back(0, 1);
    geom.faces.push_back({0, 1, 1});
    geom.subsetNames[0] = "debug";

    UgxObject u(geom);

    u.printCoordinates();
    u.printEdges();
    u.printFaces();
    CHECK(true);
}
