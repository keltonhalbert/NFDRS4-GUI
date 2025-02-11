# NFDRS4-GUI | US National Fire Danger Rating System version 4 Graphical User Interface
This Graphical User Interface (GUI) is used as an exploratory interface to the NFDRS4 models used to calculate fire danger indices. This includes the Nelson Dead Fuel Moisture Model, the Growing Season Index based Live Fuel Moisture Model, and the NFDRS4 calculator/model.  See the NFDRS4 repository for more information. 

# Dependencies
There is only one dependency not included as a git submodule in this project: GLFW3. GLFW is an open-source, cross-platform window managet for creating native platform applications. GLFW can also be compiled to Web Assembly (WASM) to run in a browser. GLFW and OpenGL are required system dependencies to build and run this application. 

All other dependencies are included as git submodules: 
    - NFDRS4: US National Fire Danger Rating System version 4
    - ImGui: Dear ImGui, an immediate mode GUI for building realtime capable cross-platform applications
    - ImPlot: A plotting library extension to ImGui, making it easy to set up interactive plots

# Building NFDRS4-GUI
## Pre-installation
Before building, you must of course clone the repository. Make sure to also clone and initialize the git submodule dependencies. 
```bash
git clone {URL}
git submodule update --init --recursive
```
## Build Steps
To build NFDRS4-GUI and its dependencies:
```bash
cmake -B build .
cmake --build build -j {N_JOBS}
```
Where ```{N_JOBS}``` represents the integer number of parallel build processes requested. 
