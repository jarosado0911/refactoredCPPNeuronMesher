# `C++` Neuron Geometry Analyzer and Mesh Generator

### ✅ CI & Quality 
[![🛠️ Build](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/cmake-single-platform.yml)
[![🧪 Doctest](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/doctest.yml/badge.svg)](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/doctest.yml)
[![Codacy Grade](https://app.codacy.com/project/badge/Grade/72da00b5a0934821b462e2e0134d4c9b)](https://app.codacy.com/gh/jarosado0911/refactoredCPPNeuronMesher/dashboard?utm_source=github.com&utm_medium=referral&utm_content=jarosado0911/refactoredCPPNeuronMesher&utm_campaign=Badge_Grade)
[![SonarCloud Alerts](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher) 
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=bugs)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
### Detailed Quality
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
### 📊 Repository Stats
![Top Language](https://img.shields.io/github/languages/top/jarosado0911/refactoredCPPNeuronMesher)
![Languages](https://img.shields.io/github/languages/count/jarosado0911/refactoredCPPNeuronMesher)
![Repo Size](https://img.shields.io/github/repo-size/jarosado0911/refactoredCPPNeuronMesher)
![Contributors](https://img.shields.io/github/contributors/jarosado0911/refactoredCPPNeuronMesher)
![PRs](https://img.shields.io/github/issues-pr/jarosado0911/refactoredCPPNeuronMesher)
![Last Commit](https://img.shields.io/github/last-commit/jarosado0911/refactoredCPPNeuronMesher)
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
