#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "externals/doctest.h"
#include "project/neurongraph.h"
#include "project/utils.h"
#include <filesystem>

TEST_CASE("Constructor 1"){
    NeuronGraph graph;
    CHECK(graph.numberOfNodes() == 0);
    CHECK(graph.numberOfEdges() == 0);
}

TEST_CASE("Constructor 2"){
    std::string filepath = getExecutableDir() + "/../data/neuron.swc";
    NeuronGraph graph(filepath);
    CHECK(graph.numberOfNodes() > 0);
    CHECK(graph.numberOfEdges() > 0);
}

TEST_CASE("Constructor 3"){
    std::string filepath = getExecutableDir() + "/../data/neuron.swc";
    NeuronGraph graph(filepath);
    NeuronGraph graph2(graph.getNodes());
    CHECK(graph2.numberOfNodes() == graph.numberOfNodes());
    CHECK(graph2.numberOfEdges() == graph.numberOfEdges());
}

TEST_CASE("Add nodes") {
    NeuronGraph graph;
    SWCNode n1{1,-1,1,0.0,0.0,0.0,1.0};
    SWCNode n2{2,1,3,1.0,0.0,0.0,1.0};
    graph.addNode(n1);
    graph.addNode(n2);
    CHECK(graph.numberOfNodes() == 2);
    CHECK(graph.numberOfEdges() == 1);
}

TEST_CASE("Read from SWC data folder") {
    std::string filepath = getExecutableDir() + "/../data/neuron.swc";
    NeuronGraph graph;
    graph.readFromFile(filepath);
    CHECK(graph.numberOfNodes() > 0);
    CHECK(graph.numberOfEdges() > 0);
}

TEST_CASE("Read all SWC files in folder") {
    std::string swcDir = getExecutableDir() + "/../data/SWC";
    REQUIRE(std::filesystem::exists(swcDir));

    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(swcDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".swc") {
            std::string filepath = entry.path().string();
            INFO("Reading: " << filepath);  // Will show file path if the test fails

            NeuronGraph g;
            CHECK_NOTHROW(g.readFromFile(filepath));
            CHECK(g.numberOfNodes() > 0);
            CHECK(g.numberOfEdges() > 0);
            count++;
        }
    }

    CHECK(count > 0);  // Ensure at least one .swc file was tested
}

TEST_CASE("Read all UGX files in folder") {
    std::string ugxDir = getExecutableDir() + "/../data/UGX";
    REQUIRE(std::filesystem::exists(ugxDir));

    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(ugxDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".ugx") {
            std::string filepath = entry.path().string();
            INFO("Reading: " << filepath);  // Will show file path if the test fails

            NeuronGraph g;
            CHECK_NOTHROW(g.readFromFileUGXorSWC(filepath));
            CHECK(g.numberOfNodes() > 0);
            CHECK(g.numberOfEdges() > 0);
            count++;
        }
    }

    CHECK(count > 0);  // Ensure at least one .swc file was tested
}

TEST_CASE("Write to output folder") {
    std::string tempFile = getExecutableDir() + "/../output/test_output/test_output.swc";
    NeuronGraph graph;
    SWCNode n1{1,-1,1,0.0,0.0,0.0,1.0};
    SWCNode n2{2,1,3,1.0,0.0,0.0,1.0};
    graph.addNode(n1);
    graph.addNode(n2);
    graph.writeToFile(graph.getNodes(), tempFile);
    NeuronGraph loaded;
    loaded.readFromFile(tempFile);
    CHECK(loaded.numberOfNodes() == 2);
    CHECK(loaded.numberOfEdges() == 1);
    auto reloaded = loaded.getNodes();
    CHECK(reloaded.at(1).x == 0.0);
    CHECK(reloaded.at(2).pid == 1);
}

TEST_CASE("Write and read UGX") {
    std::string dir = getExecutableDir();
    std::string input = dir + "/../data/neuron.swc";
    std::string output = dir + "/../output/test_output/testneuron.ugx";
    NeuronGraph g;
    g.readFromFile(input);
    g.writeToFileUGX(g.getNodes(), output);
    NeuronGraph g2;
    g2.readFromFileUGX(output);
    CHECK(g2.numberOfNodes() == g.numberOfNodes());
    CHECK(g2.numberOfEdges() == g.numberOfEdges());
}

TEST_CASE("Write to swc"){
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    g.writeToFile(getExecutableDir() + "/../output/test_output/test_write.swc");
    CHECK(true);
}

TEST_CASE("Write to ugx"){
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.ugx");
    g.writeToFileUGX(getExecutableDir() + "/../output/test_output/test_write.ugx");
    CHECK(true);
}

TEST_CASE("Read UGX or SWC") {
    std::string dir = getExecutableDir();
    NeuronGraph g1, g2;
    g1.readFromFileUGXorSWC(dir + "/../data/neuron.swc");
    g2.readFromFileUGXorSWC(dir + "/../data/neuron.ugx");
    CHECK(g1.getNodes().size() == g2.getNodes().size());
}

TEST_CASE("Topological Sort Check") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    CHECK(g.isTopologicallySorted(g.getNodes()));
}

TEST_CASE("Topological Sort Check") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    CHECK(g.isTopologicallySorted());
}

TEST_CASE("Soma Segment Detection") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    CHECK(g.hasSomaSegment(g.getNodes()));
}

TEST_CASE("Soma Segment Detection 2") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    CHECK(g.hasSomaSegment());
}

TEST_CASE("Is Soma Missing")
{
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    CHECK(!g.isSomaMissing());
}

TEST_CASE("Remove Soma Segment") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    auto nodes = g.removeSomaSegment(g.getNodes());
    CHECK(nodes.size() < g.getNodes().size());
    CHECK(g.isTopologicallySorted(nodes));
}

TEST_CASE("Remove Soma Segment 2") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    auto nodes = g.removeSomaSegment();
    CHECK(nodes.size() < g.getNodes().size());
    CHECK(g.isTopologicallySorted(nodes));
}

TEST_CASE("Set Nodes") {
    NeuronGraph g1,g2;
    g1.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    g2.setNodes(g1.getNodes());
    CHECK(g2.getNodes().size() == g1.getNodes().size());
}

TEST_CASE("Split Edges") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    auto nodes = g.removeSomaSegment(g.getNodes());
    for (int i = 0; i < 3; ++i) {
        nodes = g.splitEdges(nodes);
        CHECK(g.isTopologicallySorted(nodes));
    }
    CHECK(nodes.size() > g.getNodes().size());
}

TEST_CASE("Split Edges 2") {
    NeuronGraph g;
    g.readFromFileUGXorSWC(getExecutableDir() + "/../data/neuron.swc");
    auto nodes = g.removeSomaSegment();
    for (int i = 0; i < 8; ++i) {
        nodes = g.splitEdges();
        CHECK(g.isTopologicallySorted(nodes));
        CHECK(nodes.size() > g.getNodes().size());
        g.setNodes(nodes);
    }
}

TEST_CASE("SplitEdgesN performs multiple edge refinements") {
    std::string path = getExecutableDir() + "/../data/neuron.swc";
    NeuronGraph g;
    g.readFromFileUGXorSWC(path);

    // Remove soma segment first (required for clean refinement)
    auto nodes = g.removeSomaSegment();

    // Perform N rounds of splitting
    const int N = 5;
    auto results = g.splitEdgesN(nodes, N);

    // Check that the correct number of refinements was produced
    CHECK(results.size() == N);

    // Ensure topological order and increasing node count
    int prevSize = nodes.size();
    for (const auto& refined : results) {
        CHECK(g.isTopologicallySorted(refined));
        CHECK(refined.size() > prevSize);
        prevSize = refined.size();
    }
}

TEST_CASE("SplitEdgesN performs multiple edge refinements 2") {
    std::string path = getExecutableDir() + "/../data/neuron.swc";
    NeuronGraph g;
    g.readFromFileUGXorSWC(path);

    // Remove soma segment first (required for clean refinement)
    auto nodes = g.removeSomaSegment();

    // Perform N rounds of splitting
    const int N = 5;
    auto results = g.splitEdgesN(N);

    // Check that the correct number of refinements was produced
    CHECK(results.size() == N);

    // Ensure topological order and increasing node count
    int prevSize = nodes.size();
    for (const auto& refined : results) {
        CHECK(g.isTopologicallySorted(refined));
        CHECK(refined.size() > prevSize);
        prevSize = refined.size();
    }
}

TEST_CASE("Check and Set Soma") {
    std::string path = getExecutableDir() + "/../data/purkinjeNoSoma.swc";
    NeuronGraph g;
    g.readFromFileUGXorSWC(path);
    auto nodes = g.getNodes();
    CHECK(g.isSomaMissing(nodes));
    auto fixed = g.setSoma(nodes);
    CHECK(fixed.at(1).type == 1);
}

TEST_CASE("Check and Set Soma 2") {
    std::string path = getExecutableDir() + "/../data/purkinjeNoSoma.swc";
    NeuronGraph g;
    g.readFromFileUGXorSWC(path);
    auto nodes = g.getNodes();
    CHECK(g.isSomaMissing());
    auto fixed = g.setSoma();
    CHECK(fixed.at(1).type == 1);
}

TEST_CASE("Convert SWC file to UGX file"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.swc";
    std::string outputfile = getExecutableDir() +"/../output/test_output/swc2ugx.ugx";
    NeuronGraph g(inputfile);
    g.swc2ugx(inputfile,outputfile);
    NeuronGraph gugx(outputfile);
    CHECK(g.getNodes().size() == gugx.getNodes().size());
}

TEST_CASE("Convert UGX file to SWC file"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    std::string outputfile = getExecutableDir() +"/../output/test_output/ugx2swc.swc";
    NeuronGraph g(inputfile);
    g.ugx2swc(inputfile,outputfile);
    NeuronGraph gswc(outputfile);
    CHECK(g.getNodes().size() == gswc.getNodes().size());
}


TEST_CASE("Extract Trunks"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    NeuronGraph g(inputfile);
    auto trunks = g.getTrunks(g.getNodes(),true);
    CHECK(trunks.size() > 0);
}

TEST_CASE("Extract Trunks 2"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    NeuronGraph g(inputfile);
    auto trunks = g.getTrunks(true);
    CHECK(trunks.size() > 0);
}

TEST_CASE("Write Trunks"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    std::string outputfolder = getExecutableDir() + "/../output/test_trunks";
    NeuronGraph g(inputfile);

    // the indices of each trunk will start at 1 (true)
    // the indices of each trunk will correspond to the original geometry 0 (false)
    // if (false) you will not be able to open the geometries in ProMesh
    bool resetIndex = true;
    auto trunks = g.getTrunks(resetIndex);  
    checkFolder(outputfolder);

    for(auto& [id,trunk] : trunks){
        g.writeToFile(trunk,outputfolder + "/trunk_"+std::to_string(id)+".swc");
    }
    CHECK(trunks.size()>0);
}

TEST_CASE("Assemble Trunks"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    std::string outputfolder = getExecutableDir() + "/../output/test_output";
    NeuronGraph g(inputfile);

    // the indices of each trunk will start at 1 (true)
    // the indices of each trunk will correspond to the original geometry 0 (false)
    // if (false) you will not be able to open the geometries in ProMesh
    bool resetIndex = false; // needs to be set to false when reassembling
    auto trunks = g.getTrunks(resetIndex);  
    checkFolder(outputfolder);

    auto reassembledNodes = g.assembleTrunks(trunks);
    CHECK(reassembledNodes.size() == g.getNodes().size());
    g.setNodes(reassembledNodes);
    g.writeToFile(outputfolder+"/reassembledFromTrunks.swc");
}

TEST_CASE("Linear Resample Trunks"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    std::string outputfolder = getExecutableDir() + "/../output/neuron_trunks_resampled";
    checkFolder(outputfolder);

    NeuronGraph g(inputfile);
    g.setNodes(g.removeSomaSegment());

    // the indices of each trunk will start at 1 (true)
    // the indices of each trunk will correspond to the original geometry 0 (false)
    // if (false) you will not be able to open the geometries in ProMesh
    bool resetIndex = true; // needs to be set to false when reassembling
    auto trunks = g.getTrunks(resetIndex);
    std::map<int,std::map<int, SWCNode>> resampledTrunks;

    double delta = 2.0;

    for(auto& [id,trunk] : trunks){
        resampledTrunks[id] = g.linearSplineResampleTrunk(trunk, delta);
        g.writeToFile(resampledTrunks[id],outputfolder+"/trunk_"+std::to_string(id)+".swc");
    }
    CHECK(resampledTrunks.size()==trunks.size());
}

TEST_CASE("Get Trunk Parent Map"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    
    NeuronGraph g(inputfile);
    g.setNodes(g.removeSomaSegment());

    bool resetIndex = false;
    auto trunks = g.getTrunks(resetIndex);

    auto trunkParentMap = g.getTrunkParentMap(g.getNodes(),trunks);
    CHECK(trunkParentMap.size() != 0);
    CHECK(trunkParentMap.size() == trunks.size());
}

TEST_CASE("Assemble Resampled Trunks And Refine"){
    std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
    std::string outputfolder = getExecutableDir() + "/../output/test_resampled";
    checkFolder(outputfolder);

    NeuronGraph g(inputfile);
    g.setNodes(g.removeSomaSegment());

    bool resetIndex = false;
    auto trunks = g.getTrunks(resetIndex);

    std::map<int,std::map<int, SWCNode>> resampledTrunks;
    double delta = 64.0;

    for(int i = 0; i <= 12; ++i)
    {
        resampledTrunks = g.allLinearSplineResampledTrunks(trunks,delta);

        auto trunkParentMap = g.getTrunkParentMap(g.getNodes(),trunks);
        auto nodeSet = g.assembleTrunks(resampledTrunks,trunkParentMap);
        g.writeToFile(nodeSet,outputfolder+"/neuron_linear_resampled_refinement_"+std::to_string(i+1)+".swc");
        
        delta = delta/2.0;
        resampledTrunks.clear();
        CHECK(nodeSet.size() > 0);
    }
}

TEST_CASE("Assemble Resampled Trunks and Refine - Multiple Neurons"){
    // get paths of all .swc in the data folder
    std::string path = getExecutableDir() + "/../data/SWC";

    auto filelist = listFilesInDirectory(path);
    for(auto path : filelist)
    {
        std::cerr << path << std::endl;
    }

    std::string outputPath = getExecutableDir() + "/../output/test_assemble_multiple_neurons";
    checkFolder(outputPath);
    double delta = 1.0;

    int cnt = 1;
    for (auto path: filelist){
        // load a neuron from file path
        NeuronGraph g(path);

        // set the neuron to the preprocessed nodeSet, i.e. add soma if missing and or remove soma segment
        g.setNodes(g.preprocess(g.getNodes()));

        bool resetIndex = false;
        std::map<int,std::map<int, SWCNode>> trunks = g.getTrunks(resetIndex);

        std::map<int,std::map<int, SWCNode>> resampledTrunks;
        resampledTrunks = g.allLinearSplineResampledTrunks(trunks,delta);

        auto trunkParentMap = g.getTrunkParentMap(g.getNodes(),trunks);
        auto nodeSet = g.assembleTrunks(resampledTrunks,trunkParentMap);
        g.writeToFile(nodeSet,outputPath+"/neuron_"+std::to_string(cnt)+".swc");
        resampledTrunks.clear();
        cnt++;
    }
}