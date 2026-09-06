# 2D Physics Engine

A lightweight, high-performance 2D physics engine developed from scratch in C++20 and SDL2. The engine is built with a focus on custom mathematics, memory efficiency, and robust rigid body dynamics using the Separating Axis Theorem (SAT).

## Architecture & Technical Details

### Physics Pipeline
The core simulation pipeline is built to handle rigid body dynamics with physical realism:
1.  **Integration**: Semi-implicit Euler integration handles linear and rotational velocities, applying forces (like gravity and custom impulses) over fixed time steps.
2.  **Collision Detection**: Utilizes the **Separating Axis Theorem (SAT)**. The engine can accurately detect intersections between diverse primitive shapes (Circle-Circle, Box-Box, and Circle-Box).
3.  **Collision Resolution**: Resolves intersections by calculating the penetration depth and normal, then applying positional correction and impulse-based velocity resolution factoring in `Restitution` (bounciness) and `Mass`.

### Data Structures & Constraints
The engine relies on a custom mathematics header (`simple_math.h`) using primitives like `vec2`, `transform2D`, and intrinsic random number generation (`_rdseed32_step`).

To prevent physics destabilization, the world enforces strict physical constraints:
*   **Body Area**: Minimum of `0.0001 m²` to a maximum of `4096 m²`.
*   **Density**: Bounded between `0.5 g/cm³` and `21.4 g/cm³` to mimic real-world materials (e.g., wood to platinum).
*   **Gravity**: Defaults to Earth's `-9.81 m/s²`.

### Rendering
Rendering is handled via **SDL2**. The engine features custom CPU-driven rasterization algorithms (such as the midpoint circle algorithm) to visualize physics bodies, debug outlines, and transform origins. Box rendering uses optimized `SDL_RenderGeometry` triangles.

## Project Structure

*   **code/**: Core engine source. 
    *   `physics2D.h`: Physics pipeline, SAT algorithms, and body definitions.
    *   `simple_math.h`: Custom linear algebra math library.
    *   `game.cpp` / `game.h`: Game loop, SDL2 initialization, and rendering.
*   **misc/**: Contains build scripts, debugger configurations, and environment setups for cross-platform compatibility.
*   **third_party/**: External dependencies (SDL2, OpenGL).

## Getting Started

### Prerequisites

*   **Windows**: Visual Studio 2022 (MSVC Build Tools).
*   **Linux**: GCC or Clang with C++20 support.
*   **Dependencies (Linux)**: SDL2 development libraries.
    *   Ubuntu/Debian: `sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libgl1-mesa-dev`
    *   Fedora/RedHat: `sudo dnf install SDL2-devel SDL2_ttf-devel SDL2_image-devel mesa-libGL-devel`

### Building and Running on Windows

1.  Open the **Developer Command Prompt for VS**.
2.  Navigate to the project directory and run the build script:
    ```cmd
    misc\build.bat
    ```
3.  Execute the compiled program:
    ```cmd
    .\build\Game.exe
    ```
4.  (Optional) Run the automated tests:
    ```cmd
    misc\test.bat
    ```

### Building and Running on Linux

1.  Open a terminal and navigate to the project directory.
2.  Run the build script:
    ```bash
    ./misc/build.sh
    ```
3.  Execute the compiled program:
    ```bash
    ./build/Game
    ```
4.  (Optional) Run the automated tests:
    ```bash
    ./misc/test.sh
    ```

## Controls

Once the engine is running, you can interact with the simulation using the following controls:

*   **`C`**: Spawn a circle physics body at the current mouse cursor position.
*   **`B`**: Spawn a box physics body at the current mouse cursor position.
*   **`R` (Press)**: Clear all spawned bodies (reset the scene).
*   **`Right Mouse Button` + Drag**: Pan the camera around the world.
*   **`Mouse Wheel`**: Zoom the camera in and out.

## Development and Tools

### 4coder Integration
The project is configured for the **4coder** editor. You can load the `project.4coder` file to bind essential build and debug commands to your function keys (F1-F4).

### Visual Studio Code
A `.vscode` directory is included with pre-configured tasks and launch configurations, allowing you to build and debug the engine seamlessly using `cppdbg` (GDB) on Linux or the MSVC debugger on Windows.

## Technical Notes

*   **Strict Compilation**: The project uses strict warning flags (`-W4 -WX` on MSVC, `-Wall -Wextra -Werror` on GCC/Clang) to ensure high code quality.
*   **Concurrent Building**: On Windows, the build scripts generate unique PDB file names. This allows compilation of a new executable even while the previous build is currently running and attached to the debugger.

---

**Author:** Supriyo
