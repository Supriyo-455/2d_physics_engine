# 2D Physics Engine

A lightweight 2D physics engine built with **C++20** and **SDL2**, focusing on custom collision detection via the Separating Axis Theorem (SAT).

## 🚀 Quick Start

1.  Open the **Developer Command Prompt for VS**.
2.  Run `build.bat` to compile the engine and sync assets.
3.  Run `.\build\Game.exe` to start the application.
4.  Use `test.bat` to run the automated test suite.

## 📂 Project Structure

*   **assets/**: Game data, fonts, and textures.
*   **code/**: All source code (`.cpp`, `.h`).
*   **misc/**: Debugger configurations.
*   **notes/**: Development logs and performance improvements.
*   **third_party/**: External dependencies (SDL2, OpenGL).

## 🛠 Script Reference

| Script | Purpose |
| :--- | :--- |
| `build.bat` | Compiles the project, generates unique PDBs, and mirrors assets to the build folder. |
| `debug.bat` | Launches the executable using the `raddbg` debugger. |
| `test.bat` | Compiles and executes `all_tests.cpp`. |
| `todo.bat` | Lists all `// TODO:` comments across the source code. |

## 🏗 Technical Details

*   **Compiler Flags**: `-std:c++20 -W4 -WX` (strict warnings as errors).
*   **Debug Features**: Randomized PDB naming to allow building while the debugger is attached.
*   **Physics**: Supports Circle-Circle, Box-Box, and Circle-Box collision detection and resolution.

---
**Author:** Supriyo
