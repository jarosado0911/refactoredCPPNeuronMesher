#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "externals/doctest.h"
#include "project/ugxobject.h"
#include "project/neurongraph.h"
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

TEST_CASE("UGXObject constructor from file") {
    std::string filepath = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject u(filepath);
    CHECK_FALSE(u.getPoints().empty());
    CHECK_GE(u.getEdges().size(), 1);
}

TEST_CASE("UGXObject constructor from UgxGeometry") {
    UgxGeometry g;
    g.points[0] = {0.0, 0.0, 0.0};
    g.points[1] = {1.0, 0.0, 0.0};
    g.edges.emplace_back(0, 1);
    g.faces.push_back({0, 1, 1});
    g.subsetNames[0] = "debug";

    UgxObject u(g);
    CHECK(u.getPoints().size() == 2);
    CHECK(u.getEdges().size() == 1);
    CHECK(u.getFaces().size() == 1);
    CHECK(u.getGeometry().subsetNames.at(0) == "debug");
}

TEST_CASE("UGXObject read/write consistency") {
    std::string input = getExecutableDir() + "/../data/neuron.ugx";
    std::string output = getExecutableDir() + "/../output/test_output/test_rw.ugx";
    UgxObject u1(input);
    u1.writeUGX(output);

    UgxObject u2(output);
    CHECK(u2.getPoints().size() == u1.getPoints().size());
    CHECK(u2.getEdges().size() == u1.getEdges().size());
}

TEST_CASE("UGXObject print methods") {
    UgxGeometry g;
    g.points[0] = {1.0, 2.0, 3.0};
    g.points[1] = {4.0, 5.0, 6.0};
    g.edges.emplace_back(0, 1);
    g.faces.push_back({0, 1, 1});
    UgxObject u(g);
    CHECK_NOTHROW(u.printCoordinates());
    CHECK_NOTHROW(u.printEdges());
    CHECK_NOTHROW(u.printFaces());
}

TEST_CASE("getGeometry returns consistent data") {
    UgxGeometry g;
    g.points[5] = {1.1, 2.2, 3.3};
    g.points[8] = {3.3, 2.2, 1.1};
    g.edges.emplace_back(5, 8);
    g.faces.push_back({5, 8, 5});
    UgxObject u(g);
    const auto& returned = u.getGeometry();

    CHECK(returned.points.size() == 2);
    CHECK(returned.edges.size() == 1);
    CHECK(returned.faces.size() == 1);
}

TEST_CASE("Subset names round trip") {
    UgxGeometry g;
    g.points[0] = {0, 0, 0};
    g.points[1] = {1, 0, 0};
    g.edges.emplace_back(0, 1);
    g.subsetNames[0] = "default";

    UgxObject u(g);
    auto g2 = u.getGeometry();
    CHECK(g2.subsetNames[0] == "default");
}

TEST_CASE("Set UGX object with UGX geometry"){
    std::string input = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject g(input);
    auto before = g.getPoints().size();

    g.setGeometry(g.getGeometry());
    auto after = g.getPoints().size();
    CHECK(after == before);
}

TEST_CASE("Convert SWC nodes to UGX Geometry"){
    std::string inputswc = getExecutableDir() + "/../data/neuron.swc";
    NeuronGraph g(inputswc);
    UgxObject ug;
    auto ugxgeom = ug.convertToUGX(g.getNodes());
    CHECK(ugxgeom.radii.size() == g.getNodes().size());
    ug.setGeometry(ugxgeom);
    CHECK(ugxgeom.radii.size() == ug.getPoints().size());
    CHECK(ug.getPoints().size()== ug.getGeometry().radii.size());
    std::string output = getExecutableDir() + "/../output/test_output/test_swc_to_ugx.ugx";
    ug.writeUGX(output);
}

TEST_CASE("Read .ugx containing radii"){
    std::string inputugx = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject g(inputugx);
    auto radii = g.getGeometry().radii;
    CHECK(radii.size()==g.getGeometry().points.size());
}

TEST_CASE("Write .ugx containing radii"){
    std::string inputugx = getExecutableDir() + "/../data/neuron.ugx";
    UgxObject g(inputugx);
    std::string outputugx = getExecutableDir() + "/../output/test_output/test_ugx_with_radius.ugx";
    g.writeUGX(outputugx);
    CHECK(true);
}