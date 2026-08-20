# WormholeGateway

WormholeGateway is a free, complete source-code project for simulating hypothetical wormholes: virtual spacetime tunnels connecting two distant points.

The project models wormholes as possible gateways rather than closed endpoints. It simulates distance, curvature, stability, energy, warp factor, throat radius, traversal state, and travel time.

## Features

- C++ simulation and object management.
- C physics and utility modules.
- Native Windows GUI with a live space visualization.
- Animated stars, nebulae, wormhole tunnels, particles, and telemetry.
- Multiple wormholes with live stability and energy values.
- FPS counter and pause/resume with the `Space` key.
- CMake and Makefile build support.
- Portable shader source files for future GPU rendering.

## Project Structure

```text
WormholeGateway/
├── src/          C and C++ source files
├── include/      C headers
├── gui/          Native GUI and renderer
├── assets/       Shader, font, and texture resources
├── build/        Compiled output
├── CMakeLists.txt
├── Makefile
└── README.md
```

## Build

### Direct MinGW build

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude -Isrc -Igui `
  src/main.cpp src/Wormhole.cpp gui/GuiManager.cpp gui/Renderer.cpp `
  src/physics.c src/utils.c src/logger.c src/visualization.c `
  -o build/WormholeGateway.exe -luser32 -lgdi32 -lwinmm -mwindows
```

### Make

```powershell
make
```

### CMake

```powershell
cmake -S . -B build/cmake
cmake --build build/cmake --config Release
```

## Run

```powershell
.\build\WormholeGateway.exe
```

The application opens as a native Windows GUI without a console window.

## Controls

- `Space`: pause or resume the simulation.
- Close the window: exit the application.

## Free Use

This repository contains the complete project source code. It is provided free of charge, and everyone is allowed to use, study, copy, modify, and share it for personal or commercial projects.

There is no warranty. Use the project and its source code at your own risk.

## Note

This is an educational and visual simulation of hypothetical spacetime physics. It is not a real wormhole engine or a scientific prediction.

## Credit 
This project was developed by RedHarow 

discord Username : 0xrdo

github : https://github.com/redharlowcore
