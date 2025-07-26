# `C++` Neuron Geometry Analyzer and Mesh Generator
### ✅ CI & Quality 
[![🛠️ Build](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/cmake-single-platform.yml)
[![🧪 Doctest](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/doctest.yml/badge.svg)](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/doctest.yml)
[![Codacy Grade](https://app.codacy.com/project/badge/Grade/72da00b5a0934821b462e2e0134d4c9b)](https://app.codacy.com/gh/jarosado0911/refactoredCPPNeuronMesher/dashboard?utm_source=github.com&utm_medium=referral&utm_content=jarosado0911/refactoredCPPNeuronMesher&utm_campaign=Badge_Grade)
[![SonarCloud Alerts](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher) 
### *️⃣ Detailed Quality
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=bugs)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
### 📊 Repository Statistics
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
![Top Language](https://img.shields.io/github/languages/top/jarosado0911/refactoredCPPNeuronMesher)
![Languages](https://img.shields.io/github/languages/count/jarosado0911/refactoredCPPNeuronMesher)
![Repo Size](https://img.shields.io/github/repo-size/jarosado0911/refactoredCPPNeuronMesher)
![Contributors](https://img.shields.io/github/contributors/jarosado0911/refactoredCPPNeuronMesher)
![PRs](https://img.shields.io/github/issues-pr/jarosado0911/refactoredCPPNeuronMesher)
![Last Commit](https://img.shields.io/github/last-commit/jarosado0911/refactoredCPPNeuronMesher)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
### 📦 GitHub Metadata
![License: MIT](https://img.shields.io/github/license/jarosado0911/refactoredCPPNeuronMesher)
![Release](https://img.shields.io/github/v/release/jarosado0911/refactoredCPPNeuronMesher?include_prereleases)
![Stars](https://img.shields.io/github/stars/jarosado0911/refactoredCPPNeuronMesher?style=social)
![Forks](https://img.shields.io/github/forks/jarosado0911/refactoredCPPNeuronMesher?style=social)
### 📡 Static Analysis Summary
[![SonarCloud](https://sonarcloud.io/images/project_badges/sonarcloud-light.svg)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Quality Gate](https://sonarcloud.io/api/project_badges/quality_gate?project=jarosado0911_refactoredCPPNeuronMesher)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
## Overview

**refactoredCPPNeuronMesher** is a C++ toolkit for analyzing neuron geometries and generating computational meshes from neuron morphology data (primarily in the `.swc` format). The project provides both command-line tools for mesh operations, and a lightweight OpenGL-based 3D viewer for visualizing neuron structures.

Main features include:
- Parsing and analyzing neuron morphologies in `.swc` format.
- Automated mesh generation, refinement, and trunk extraction for neuron models.
- A fast, interactive 3D neuron viewer with rotation, zoom, and multiple visualization modes.
- Python bindings (`neurongraph` shared library).
- Modular CMake build system with continuous integration and quality analysis.
- Example scripts and utilities for advanced mesh and data manipulation.

---

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Build Instructions](#build-instructions)
- [Using the Makefile](#using-the-makefile)
- [Python Bindings and Notebook](#python-bindings-and-notebook)
  - [The NeuronGraph Class](#the-neurongraph-class)
- [Running the Tools](#running-the-tools)
- [Neuron Viewer](#neuron-viewer)
- [SWC File Format](#swc-file-format)
- [Troubleshooting](#troubleshooting)
- [License](#license)

---

## Features

- **Mesh Generation**: Create and refine computational meshes from neuron morphology data.
- **Trunk Extraction**: Isolate the main trunk or branches of neuron models.
- **Batch Refinement**: Split and refine neuron sets for large datasets.
- **3D Visualization**: Real-time, interactive OpenGL viewer for `.swc` files.
- **Python Interoperability**: Shared library for Python via `pybind11`.
- **Cross-Platform**: Designed for Linux (WSL supported), with modular CMake build.
- **Quality & Security**: Integrated with SonarCloud and Codacy for code analysis and CI.

---

## Prerequisites

- **Compiler**: C++17 compatible (e.g., `g++`, `clang++`)
- **CMake**: Version 3.10+
- **Dependencies**:
    - `tinyxml2` (for XML parsing)
    - `GLFW` (for 3D viewer)
    - `GL` and `GLU` (OpenGL core and utilities)
    - `X11` (if using WSL or X11-based systems)
- **Python** (for using Python bindings via `pybind11`)

### Install dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential cmake libtinyxml2-dev libglfw3-dev libglu1-mesa-dev libgl1-mesa-dev x11-utils
```

**For WSL Users**:
1. Install an X Server on Windows, like **VcXsrv** or **Xming**.
2. Launch your X server and allow access from 127.0.0.1.
3. Then in your WSL terminal:

```bash
export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0
export LIBGL_ALWAYS_INDIRECT=1
```

---

## Build Instructions

Clone the repository and build using CMake:

```bash
git clone https://github.com/jarosado0911/refactoredCPPNeuronMesher.git
cd refactoredCPPNeuronMesher
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### To manually build the 3D SWC Viewer (standalone):

```bash
cd scripts/app_neuronviewer
g++ -O3 -std=c++17 -o swc_viewer swc_viewer.cpp -lGL -lGLU -lglfw
```

---

## Using the Makefile

This repository includes a top-level Makefile that provides convenient shortcuts for building, testing, cleaning, and installing the project and its Python modules.

### Common Targets

- `make` or `make build`: Configure and build the project using CMake in the `./build/` directory. Binaries are placed in `./bin/`.
- `make clean`: Remove the `build/` and `bin/` directories and all Python cache/egg-info files.
- `make ctest`: Build the project and run CTest-based unit tests (legacy test system).
- `make doctest`: Build (if necessary) and run all C++ doctests (recommended for full coverage).
- `make all`: Run build, doctest, and then clean up intermediate files.
- `make rebuild` or `make remake`: Clean everything, then build from scratch.
- `make install`: Install required Python dependencies using `requirements.txt`.
- `make installe`: Install the Python package in "editable" development mode.
- `make testpy`: Run Python tests in `python_package/test/` with pytest.
- `make help`: Display all available make targets with a short description.

### Example Usage

```bash
# Build the project
make

# Run all C++ doctests
make doctest

# Clean the build and binary directories
make clean

# Install Python requirements
make install

# Install the Python package in editable mode (for development)
make installe

# Run Python tests
make testpy
```

For advanced or troubleshooting usage, consult the help target:

```bash
make help
```

The Makefile is designed to streamline both C++ and Python development workflows in this repository.

---
## The `NeuronGraph` Class

The `NeuronGraph` class is the central data structure for representing, manipulating, and analyzing neuron morphologies within the toolkit. It models a neuron as a directed graph, where each node represents a structural element (such as a soma or neurite segment) and edges capture the connectivity between these elements. Internally, the class maintains a map of nodes (each keyed by a unique integer identifier and storing 3D position, radius, and type information) and a map of edges (where each node index maps to a list of its child node indices). The `NeuronGraph` class provides multiple constructors for initializing an empty graph, loading from SWC or UGX morphology files, or directly from a node set. Its extensive API supports reading and writing morphologies, topological sorting, soma detection and manipulation, preprocessing (such as cleaning or normalizing the graph), and various forms of mesh refinement and trunk extraction. Key methods include `readFromFile`, `writeToFile`, `getNodes`, `setNodes`, `preprocess`, `splitEdges`, and `getTrunks`, among others. The class is designed for both high-level batch processing and fine-grained interactive use, and is fully exposed to Python via pybind11 bindings, making it accessible in scripts, pipelines, or Jupyter notebooks. Typical use cases include loading neuron data, processing or refining its structure, analyzing branches and connectivity, and exporting results for visualization or simulation.

---
## Python Bindings and Notebook

This repository provides Python bindings for its core C++ neuron analysis and meshing functionality through a shared library called `neurongraph`. The bindings are implemented using `pybind11`, enabling Python code to directly access high-performance C++ routines for neuron graph construction, file I/O, and mesh operations. These bindings are ideal for users who want to leverage the speed of C++ but prefer the flexibility and interactive environment of Python.

To illustrate the use of these bindings, a Jupyter notebook is provided: [Tutorial - NeuronGraph.ipynb](https://github.com/jarosado0911/refactoredCPPNeuronMesher/blob/main/python_package/notebooks/Tutorial%20-%20NeuronGraph.ipynb). This notebook demonstrates how to import and use the `neurongraph` module, including constructing neuron graphs, reading morphology files, and performing analyses in Python. The notebook also shows how to integrate with Python scientific libraries such as `numpy`, `trimesh`, and `pythreejs` for advanced analysis and interactive 3D visualization. By following the notebook, users can learn how to harness the full power of the C++ backend within their Python-based computational workflows.

---

## Running the Tools

### Command-Line Utilities

Compiled binaries are placed in the `bin/` directory. Main executables include:

- **splitrefine**: Refines neuron SWC files (mesh subdivision)
- **splitrefineset**: Batch refinement for a set of neurons
- **extracttrunks**: Extracts main trunk/branches from SWC files

Example usage:

```bash
./bin/splitrefine path/to/input.swc
./bin/splitrefineset path/to/input.swc
./bin/extracttrunks path/to/input.swc
```

Output is typically written to the `output/` directory, with filenames based on the input SWC or a timestamp-based hash.

---

## Neuron Viewer

A lightweight 3D OpenGL-based viewer to render `.swc` neuron morphology files with interactive features:

- **Rotation**: Mouse drag
- **Zoom**: Scroll wheel
- **Visualization Modes**: Keys `1` to `6` for switching render styles
- **Colored nodes/edges**: By type and radius
- **Lighting & Bounding Box**: Optional enhancements

### Running the Viewer

```bash
./swc_viewer path/to/neuron.swc
```

#### Interactive Controls

- 🖱 **Left-click + drag**: Rotate the neuron
- 🔍 **Scroll wheel**: Zoom in/out
- 🔢 **Number keys (1–6)**:
    - `1`: edges only (uniform)
    - `2`: edges + uniform nodes
    - `3`: nodes only (uniform size)
    - `4`: nodes only (by radius)
    - `5`: edges by radius
    - `6`: full enhanced (edges by radius, lighting, etc.)

---

## SWC File Format

The standard SWC format is used for neuron morphologies:

```
# id type x y z radius parent
1 1 0.0 0.0 0.0 1.0 -1
2 3 5.0 0.0 0.0 0.5 1
3 3 10.0 0.0 0.0 0.4 2
...
```

---

## Troubleshooting

- **Window doesn't appear**: Ensure `DISPLAY` is set and X server is running.
- **Blank window**: Use `LIBGL_ALWAYS_INDIRECT=1` in WSL.
- **Missing cylinders**: May be due to overlapping points or zero-length segments.
- **Build errors**: Ensure all dependencies are installed and CMake is version 3.10 or newer.

---

## License

This project is released under the [MIT License](LICENSE).

&copy; 2025 James Rosado

---
