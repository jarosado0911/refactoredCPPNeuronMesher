Welcome to the documentation and website for the refactoredCPPNeuronMesher project!

- [GitHub Repository](https://github.com/jarosado0911/refactoredCPPNeuronMesher)
- [About](#about)
- [Usage](#usage)
- [Installation](#installation)
- [Examples](#examples)
- [MetaData](#metadata)

## About
CPPNeuronMesher is a C++ project designed for the geometric analysis and mesh generation of neuron morphologies, providing tools for advanced neuroscience modeling and simulation. The repository features robust CI/CD pipelines, extensive quality checks, and detailed documentation to support reproducibility and collaborative development. CPPNeuronMesher is a C++ toolkit for analyzing neuron geometries and generating computational meshes from neuron morphology data (primarily in the `.swc` format). The project provides both command-line tools for mesh operations, and a lightweight OpenGL-based 3D viewer for visualizing neuron structures.

Main features include:
- Parsing and analyzing neuron morphologies in `.swc` format.
- Automated mesh generation, refinement, and trunk extraction for neuron models.
- A fast, interactive 3D neuron viewer with rotation, zoom, and multiple visualization modes.
- Python bindings (`neurongraph` shared library).
- Modular CMake build system with continuous integration and quality analysis.
- Example scripts and utilities for advanced mesh and data manipulation.

## Examples
In this section we discuss the example scripts and show the `neuronviewer` application I designed for visualizing `.swc` and `.ugx` neuron geometry specifications.
### NeuronViewer: A 3D Visualization Tool for Neuronal Morphology
`neuronviewer` is a lightweight, OpenGL-powered interactive application designed for real-time visualization of neuron morphologies encoded in `.swc` and `.ugx` formats. Built with C++ and GLFW, it allows researchers to explore neuron structures through intuitive 3D navigation and rich rendering options.

🧠 Core Features
Real-time 3D rendering of .swc neuron files using OpenGL.
- Interactive controls:
- Rotate with mouse drag
- Zoom with scroll wheel
- Pan and explore 3D space
- Multi-mode rendering with six distinct visual styles:
- Edges only (uniform)
- Nodes only (uniform or by radius)
- Edges and nodes (with or without radius-based scaling)
- Automatic lighting, bounding box, and color-coded nodes by SWC type.
- Dynamic help window showing usage instructions and current file metadata.

🎥 Demonstration
<video width="640" height="360" autoplay loop muted playsinline controls> <source src="media/neuronviewer.mp4" type="video/mp4"> Your browser does not support the video tag. </video> <p><em>Demonstration of the neuron viewer in action.</em></p>
🧰 Technical Highlights
- Utilizes GLFW for window and input management.
- Uses GLU to draw spheres and cylinders representing nodes and neurites.
- Employs a modular rendering pipeline with lighting and material settings.
- Efficient font rendering using stb_easy_font for overlay/help text.
- Help window rendered via a second GLFW context (renderhelpwindow.cpp) with orthographic projection and line-by-line formatting.

## MetaData
### ✅ CI & Quality 
The CI & Quality badges show the current build status, test results, and automated code quality checks, giving a quick snapshot of whether the project is compiling, passing its tests, and meeting quality standards.

[![🛠️ Build](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/cmake-single-platform.yml)
[![🧪 Doctest](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/doctest.yml/badge.svg)](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/doctest.yml)
[![Codacy Grade](https://app.codacy.com/project/badge/Grade/72da00b5a0934821b462e2e0134d4c9b)](https://app.codacy.com/gh/jarosado0911/refactoredCPPNeuronMesher/dashboard?utm_source=github.com&utm_medium=referral&utm_content=jarosado0911/refactoredCPPNeuronMesher&utm_campaign=Badge_Grade)
[![SonarCloud Alerts](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher) 
![Code Scanning - Action](https://github.com/jarosado0911/refactoredCPPNeuronMesher/actions/workflows/codeql.yml/badge.svg)
### *️⃣ Detailed Quality
The Detailed Quality badges provide in-depth information about the project’s software health, including reliability, security, vulnerabilities, bugs, and code smells, as assessed by SonarCloud. These metrics help developers and users identify potential risks and areas for improvement in the codebase.

[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=bugs)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
### 📊 Repository Statistics
The Repository Statistics badges display key metrics about the project's codebase and activity, such as the main programming language, total languages used, repository size, number of contributors, open pull requests, and the date of the last commit. These badges provide a quick overview of the project's scale, diversity, and recent development activity.

[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
![Top Language](https://img.shields.io/github/languages/top/jarosado0911/refactoredCPPNeuronMesher)
![Languages](https://img.shields.io/github/languages/count/jarosado0911/refactoredCPPNeuronMesher)
![Repo Size](https://img.shields.io/github/repo-size/jarosado0911/refactoredCPPNeuronMesher)
![Contributors](https://img.shields.io/github/contributors/jarosado0911/refactoredCPPNeuronMesher)
![PRs](https://img.shields.io/github/issues-pr/jarosado0911/refactoredCPPNeuronMesher)
![Last Commit](https://img.shields.io/github/last-commit/jarosado0911/refactoredCPPNeuronMesher)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=jarosado0911_refactoredCPPNeuronMesher&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
### 📦 GitHub Metadata
The GitHub Metadata badges highlight important repository details such as the project's license, latest release version, star and fork counts, which indicate popularity and community engagement. These badges help users quickly gauge the project's openness, activity, and level of public interest.

![License: MIT](https://img.shields.io/github/license/jarosado0911/refactoredCPPNeuronMesher)
![Release](https://img.shields.io/github/v/release/jarosado0911/refactoredCPPNeuronMesher?include_prereleases)
![Stars](https://img.shields.io/github/stars/jarosado0911/refactoredCPPNeuronMesher?style=social)
![Forks](https://img.shields.io/github/forks/jarosado0911/refactoredCPPNeuronMesher?style=social)
### 📡 Static Analysis Summary
The Static Analysis Summary badges indicate the results of automated code analysis performed by tools like SonarCloud, highlighting overall code quality and whether the project passes essential quality gates. These provide a quick status check for maintainers and users, and you can view the full analysis details on [SonarCloud](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher).

[![SonarCloud](https://sonarcloud.io/images/project_badges/sonarcloud-light.svg)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)
[![Quality Gate](https://sonarcloud.io/api/project_badges/quality_gate?project=jarosado0911_refactoredCPPNeuronMesher)](https://sonarcloud.io/summary/new_code?id=jarosado0911_refactoredCPPNeuronMesher)

