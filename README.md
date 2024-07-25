# Game of Life in Raylib (C)

I implemented [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) using [Raylib](https://github.com/raysan5/raylib), a C library for simple 2D game development.

## Requirements

- **Raylib**: A C library for simple 2D game development.

## Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/SamarthPyati/Game-of-Life-in-C.git
   cd Game-of-Life-in-C
   ```

2. **Install Raylib:**
   Follow the installation instructions on the [Raylib website](https://www.raylib.com/).

3. **Compile the Code:**
   You can compile the project using a C compiler. If you're using GCC on Windows, use the following command:
   ```bash
   gcc -o game_of_life main.c -lraylib -lgdi32 -lopengl32 -lm
   ```
   Make sure to replace `main.c` with the appropriate file name if necessary.

## Usage

1. **Run the Program:**
   - Build the program.
   - Execute the compiled binary:
     ```bash
     ./game_of_life
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

## Upcoming Features

- [ ] Epic Music Background
- [ ] Save Game state in files
- [ ] Colorful gradients to hallucinate
- [ ] More user friendly UI

## Author
Samarth Sanjay Pyati

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
- [Raylib](https://www.raylib.com/)
