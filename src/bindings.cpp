#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "neurongraph.h"

namespace py = pybind11;

PYBIND11_MODULE(neurongraph, m){
    py::class_<SWCNode>(m, "SWCNode")
        .def(py::init<>())
        .def_readwrite("id", &SWCNode::id)
        .def_readwrite("pid", &SWCNode::pid)
        .def_readwrite("type", &SWCNode::type)
        .def_readwrite("x", &SWCNode::x)
        .def_readwrite("y", &SWCNode::y)
        .def_readwrite("z", &SWCNode::z)
        .def_readwrite("radius", &SWCNode::radius);

    py::class_<NeuronGraph>(m, "NeuronGraph")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def(py::init<const std::map<int, SWCNode>&>())

        .def("addNode", &NeuronGraph::addNode)
        .def("setNodes", &NeuronGraph::setNodes)
        .def("getNodes", &NeuronGraph::getNodes)

        .def("readFromFile", &NeuronGraph::readFromFile)
        .def("writeToFile", py::overload_cast<const std::map<int, SWCNode>&, const std::string&>(&NeuronGraph::writeToFile))
        .def("writeToFile", py::overload_cast<const std::string&>(&NeuronGraph::writeToFile))
        .def("readFromFileUGX", &NeuronGraph::readFromFileUGX)
        .def("readFromFileUGXorSWC", &NeuronGraph::readFromFileUGXorSWC)
        .def("writeToFileUGX", py::overload_cast<const std::map<int, SWCNode>&, const std::string&>(&NeuronGraph::writeToFileUGX))
        .def("writeToFileUGX", py::overload_cast<const std::string&>(&NeuronGraph::writeToFileUGX))

        .def("swc2ugx", &NeuronGraph::swc2ugx)
        .def("ugx2swc", &NeuronGraph::ugx2swc)

        .def("numberOfNodes", &NeuronGraph::numberOfNodes)
        .def("numberOfEdges", &NeuronGraph::numberOfEdges)

        .def("isTopologicallySorted", py::overload_cast<>(&NeuronGraph::isTopologicallySorted, py::const_))
        .def("topologicalSort", py::overload_cast<>(&NeuronGraph::topologicalSort, py::const_))
        .def("hasSomaSegment", py::overload_cast<>(&NeuronGraph::hasSomaSegment, py::const_))
        .def("isSomaMissing", py::overload_cast<>(&NeuronGraph::isSomaMissing, py::const_))
        .def("removeSomaSegment", py::overload_cast<>(&NeuronGraph::removeSomaSegment, py::const_))
        .def("setSoma", py::overload_cast<>(&NeuronGraph::setSoma, py::const_))
        .def("preprocess", &NeuronGraph::preprocess)

        .def("splitEdges", py::overload_cast<>(&NeuronGraph::splitEdges, py::const_))
        .def("splitEdgesN", py::overload_cast<int>(&NeuronGraph::splitEdgesN, py::const_))

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
