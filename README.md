# Cellular Automata in Raylib (C)

I implemented [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) and [Brain`s Brain](https://en.wikipedia.org/wiki/Brian%27s_Brain) using [Raylib](https://github.com/raysan5/raylib).

## Requirements

- **Raylib**: A C library for simple 2D game development.

## Feature

- Conway's Game of Life implementation
- Brian's Brain implementation
- Camera controls (pan and zoom)
- Grid randomization and clearing
- Ability to switch between rule sets

## Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/SamarthPyati/Cellular_Automata_In_Raylib.git
   cd Cellular_Automata_In_RayLib
   ```

2. **Install Raylib:**
   Follow the installation instructions on the [Raylib website](https://www.raylib.com/).

3. **Compile the Code:**
   - You can compile the project using a C compiler. If you're using GCC on Windows, use the following command:
   ```bash
   gcc -o cellular_automata gol.c -lraylib -lgdi32 -lopengl32 -lm
   ```

   - On linux / mac operating systems: 
    ```bash
   gcc $(pkg-config --libs --cflags raylib) -lm -o cellular_automata gol.c 
   ```  

## Usage

1. **Run the Program:**
   - Build the program.
   - Execute the compiled binary:
     ```bash
     ./cellular_automata
     ```

2. **Controls:**

   | Key Combination | Action |
   |-----------------|--------|
   | Arrow Keys      | Move the camera |
   | Mouse Wheel     | Zoom in and out |
   | Left Mouse Button | Toggle cells |
   | SPACE           | Pause/Unpause the simulation |
   | R               | Randomize the grid |
   | C               | Clear the grid |
   | Z               | Reset camera position and rotation |
   | Left Shift + Z  | Reset camera position, rotation, and zoom |
   | B               | Switch the Rules   |
   | Esc             | Quit the program | 

## Upcoming Features

- [ ] Save Game state in files
- [ ] Colorful gradients to hallucinate
- [ ] More user friendly UI

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

