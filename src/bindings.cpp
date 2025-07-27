/**
 * @file bindings.cpp
 * @brief Python bindings for the CPPNeuronMesher library using pybind11
 * @author CPPNeuronMesher Project
 * @date 2024
 * 
 * This file provides Python bindings for the NeuronGraph class and SWCNode structure,
 * enabling Python users to access the full functionality of the CPPNeuronMesher library.
 * The bindings expose all major operations including file I/O, morphology analysis,
 * preprocessing, mesh operations, and trunk analysis.
 * 
 * The Python module can be imported as:
 * @code{.py}
 * import neurongraph
 * graph = neurongraph.NeuronGraph()
 * @endcode
 * 
 * @see NeuronGraph for the main class documentation
 * @see SWCNode for the node structure documentation
 */

 #include <pybind11/pybind11.h>
 #include <pybind11/stl.h>
 #include "neurongraph.h"
 
 namespace py = pybind11;
 
 /**
  * @brief Python module definition for neurongraph
  * @param m The pybind11 module object
  * 
  * This function defines the Python module 'neurongraph' and exposes all C++ classes
  * and methods to Python. The module provides two main classes:
  * - SWCNode: Represents individual neuron nodes with spatial and topological data
  * - NeuronGraph: Main class for neuron morphology analysis and processing
  * 
  * All method signatures are preserved from C++ to Python, with automatic type
  * conversion handled by pybind11 for standard types (int, double, string, maps, vectors).
  */
 PYBIND11_MODULE(neurongraph, m){
     /**
      * @brief Python binding for SWCNode structure
      * 
      * Exposes the SWCNode C++ structure to Python, providing direct access to all
      * neuron node properties. SWCNode represents a single point in a neuron morphology
      * with spatial coordinates, topological relationships, and morphological type.
      * 
      * Python usage:
      * @code{.py}
      * node = neurongraph.SWCNode()
      * node.id = 1
      * node.pid = -1  # Root node
      * node.type = 1  # Soma
      * node.x, node.y, node.z = 0.0, 0.0, 0.0
      * node.radius = 5.0
      * @endcode
      * 
      * @note All attributes are read-write from Python
      * @see SWCNode C++ structure for detailed field descriptions
      */
     py::class_<SWCNode>(m, "SWCNode")
         .def(py::init<>(), "Default constructor creating an empty SWCNode")
         .def_readwrite("id", &SWCNode::id, "Unique node identifier")
         .def_readwrite("pid", &SWCNode::pid, "Parent node ID (-1 for root nodes)")
         .def_readwrite("type", &SWCNode::type, "Morphological type (1=soma, 2=axon, 3=dendrite, etc.)")
         .def_readwrite("x", &SWCNode::x, "X coordinate in 3D space")
         .def_readwrite("y", &SWCNode::y, "Y coordinate in 3D space")
         .def_readwrite("z", &SWCNode::z, "Z coordinate in 3D space")
         .def_readwrite("radius", &SWCNode::radius, "Node radius for cylindrical representation");
 
     /**
      * @brief Python binding for NeuronGraph class
      * 
      * Exposes the complete NeuronGraph C++ class to Python, providing access to all
      * neuron morphology analysis, processing, and I/O operations. This is the main
      * class for working with neuron data in Python.
      * 
      * Python usage examples:
      * @code{.py}
      * # Create empty graph
      * graph = neurongraph.NeuronGraph()
      * 
      * # Load from file
      * graph = neurongraph.NeuronGraph("neuron.swc")
      * 
      * # Create from node dictionary
      * nodes = {1: node1, 2: node2, ...}
      * graph = neurongraph.NeuronGraph(nodes)
      * @endcode
      */
     py::class_<NeuronGraph>(m, "NeuronGraph")
         .def(py::init<>(), "Default constructor creating an empty NeuronGraph")
         .def(py::init<std::string>(), "Constructor that loads neuron data from file", py::arg("filename"))
         .def(py::init<const std::map<int, SWCNode>&>(), "Constructor from node dictionary", py::arg("nodeSet"))
 
         // Core node manipulation methods
         .def("addNode", &NeuronGraph::addNode, "Add a single node to the graph", py::arg("node"))
         .def("setNodes", &NeuronGraph::setNodes, "Replace all nodes with new node set", py::arg("nodeSet"))
         .def("getNodes", &NeuronGraph::getNodes, "Get all nodes as a dictionary")
 
         // File I/O methods
         .def("readFromFile", &NeuronGraph::readFromFile, "Load neuron data from SWC file", py::arg("filename"))
         .def("writeToFile", py::overload_cast<const std::map<int, SWCNode>&, const std::string&>(&NeuronGraph::writeToFile),
              "Write node set to SWC file", py::arg("nodeSet"), py::arg("filename"))
         .def("writeToFile", py::overload_cast<const std::string&>(&NeuronGraph::writeToFile),
              "Write current graph to SWC file", py::arg("filename"))
         .def("readFromFileUGX", &NeuronGraph::readFromFileUGX, "Load neuron data from UGX file", py::arg("filename"))
         .def("readFromFileUGXorSWC", &NeuronGraph::readFromFileUGXorSWC, 
              "Auto-detect format and load from SWC or UGX file", py::arg("filename"))
         .def("writeToFileUGX", py::overload_cast<const std::map<int, SWCNode>&, const std::string&>(&NeuronGraph::writeToFileUGX),
              "Write node set to UGX file", py::arg("nodeSet"), py::arg("filename"))
         .def("writeToFileUGX", py::overload_cast<const std::string&>(&NeuronGraph::writeToFileUGX),
              "Write current graph to UGX file", py::arg("filename"))
 
         // Format conversion utilities
         .def("swc2ugx", &NeuronGraph::swc2ugx, "Convert SWC file to UGX format", 
              py::arg("inputfile"), py::arg("outputfile"))
         .def("ugx2swc", &NeuronGraph::ugx2swc, "Convert UGX file to SWC format", 
              py::arg("inputfile"), py::arg("outputfile"))
 
         // Graph analysis methods
         .def("numberOfNodes", &NeuronGraph::numberOfNodes, "Get total number of nodes in the graph")
         .def("numberOfEdges", &NeuronGraph::numberOfEdges, "Get total number of edges in the graph")
 
         // Topology validation and correction methods
         .def("isTopologicallySorted", py::overload_cast<>(&NeuronGraph::isTopologicallySorted, py::const_),
              "Check if nodes are topologically sorted (parents have smaller IDs than children)")
         .def("topologicalSort", py::overload_cast<>(&NeuronGraph::topologicalSort, py::const_),
              "Sort nodes topologically ensuring proper parent-child ID ordering")
         
         // Soma analysis and correction methods
         .def("hasSomaSegment", py::overload_cast<>(&NeuronGraph::hasSomaSegment, py::const_),
              "Check if neuron has multiple soma nodes (soma segment)")
         .def("isSomaMissing", py::overload_cast<>(&NeuronGraph::isSomaMissing, py::const_),
              "Check if neuron is missing a soma node")
         .def("removeSomaSegment", py::overload_cast<>(&NeuronGraph::removeSomaSegment, py::const_),
              "Remove soma segments by consolidating multiple soma nodes into one")
         .def("setSoma", py::overload_cast<>(&NeuronGraph::setSoma, py::const_),
              "Assign soma to neuron by converting first root node to soma type")
         .def("preprocess", &NeuronGraph::preprocess, 
              "Preprocess neuron data (fix soma issues, ensure topological sorting)", py::arg("nodeSet"))
 
         // Mesh refinement operations
         .def("splitEdges", py::overload_cast<>(&NeuronGraph::splitEdges, py::const_),
              "Split all edges by inserting midpoint nodes for mesh refinement")
         .def("splitEdgesN", py::overload_cast<int>(&NeuronGraph::splitEdgesN, py::const_),
              "Perform N iterations of edge splitting for progressive refinement", py::arg("N"))
 
         .def("getTrunks", py::overload_cast<bool>(&NeuronGraph::getTrunks, py::const_))
         .def("getTrunkParentMap", &NeuronGraph::getTrunkParentMap)
         .def("assembleTrunks", py::overload_cast<const std::map<int, std::map<int, SWCNode>>&>(&NeuronGraph::assembleTrunks, py::const_))
         .def("assembleTrunks", py::overload_cast<const std::map<int, std::map<int, SWCNode>>&, const std::map<int, int>&>(&NeuronGraph::assembleTrunks))
 
         .def("linearSplineResampleTrunk", &NeuronGraph::linearSplineResampleTrunk)
         .def("allLinearSplineResampledTrunks", &NeuronGraph::allLinearSplineResampledTrunks)
         .def("cubicSplineResampleTrunk", &NeuronGraph::cubicSplineResampleTrunk)
         .def("allCubicSplineResampledTrunks", &NeuronGraph::allCubicSplineResampledTrunks)
         .def("generateRefinements",py::overload_cast<const std::map<int, SWCNode>&, double&, int&, std::string&>(&NeuronGraph::generateRefinements),
      py::arg("nodeSet"), py::arg("delta"), py::arg("N"), py::arg("method"))
         .def("generateRefinements",py::overload_cast<double&, int&, std::string&>(&NeuronGraph::generateRefinements),
      py::arg("delta"), py::arg("N"), py::arg("method"));
 }
 