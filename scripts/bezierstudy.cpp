#include "neurongraph.h"
#include "ugxobject.h"
#include "utils.h"
#include <chrono>
#include <functional>
#include <sstream>

int main( int argc, char* argv[]){

	std::string inputfile = getExecutableDir() + "/../data/neuron.ugx";
	std::string outputfolder = getExecutableDir() + "/../output/bezierstudy";
	checkFolder(outputfolder);

	NeuronGraph g(inputfile);
	g.setNodes(g.removeSomaSegment());
	for(int i = 0; i<=8; ++i){g.setNodes(g.splitEdges());}

	auto subgraphs = g.extractBranchSubgraphs();
	double insetFactor = 0.25;
	int numberOfBezierPoints = 30;

	NeuronGraph g0;

	for(auto& [bid,subg] : subgraphs){
        	auto beziercurves = g0.smoothBranchWithBezier(subg,insetFactor,numberOfBezierPoints);
	        for(auto& [id,bez] : beziercurves){
        	    std::string outputfile = outputfolder + "/subgraph_"+std::to_string(bid)+"_"+std::to_string(id)+".swc";
		    g0.writeToFile(bez,outputfile);
        }
    }
	
	UgxGeometry combinedBezierObj;
	UgxObject tempBezierObj;

	std::string inputBezier = getExecutableDir() +  "/../output/bezierstudy";
    	const auto fileNames = listFilesInDirectory(inputBezier);
    	for(auto f : fileNames){
        	NeuronGraph bezierPath(f);
        	auto pft = bezierPath.pftFromPath(bezierPath.getNodes(), 16);
        	combinedBezierObj = tempBezierObj.addUGXGeometry(combinedBezierObj,pft.getGeometry());
    	}

    	outputfolder = getExecutableDir() + "/../output";
    	tempBezierObj.setGeometry(combinedBezierObj);
    	tempBezierObj.writeUGX(outputfolder+"/BezierStudyCombined.ugx");

    return 0;
}
