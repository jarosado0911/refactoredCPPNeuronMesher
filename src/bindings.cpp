#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "neurongraph.h"

namespace py = pybind11;

PYBIND11_MODULE(neurongraph, m){
    py::class_<SWCNode>(m,"SWCNode")
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
        .def("readFromFile", &NeuronGraph::readFromFile)
        .def("writeToFile", static_cast<void (NeuronGraph::*)(const std::string&)>(&NeuronGraph::writeToFile))
        .def("numberOfNodes", &NeuronGraph::numberOfNodes)
        .def("numberOfEdges", &NeuronGraph::numberOfEdges)
	.def("getNodes",&NeuronGraph::getNodes);
}
	
