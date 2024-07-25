#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h" 

// WINDOW 
const size_t WWIDTH = 1440;
const size_t WHEIGHT = 900;

#define BG_COLOR (Color) {39, 41, 40, 255}

// CELL
typedef struct 
{
    Vector2 pos;
    bool alive;
} Cell;

#define CELL_SIZE 5                 

// GRID
#define GRID_WIDTH (WWIDTH / CELL_SIZE)
#define GRID_HEIGHT (WHEIGHT / CELL_SIZE)
#define GRID_LINE_THICKNESS 0.2f
#define GRID_LINE_COLOR RAYWHITE
#define GRID_AXIS_THICKNESS 0.9f
#define GRID_AXIS_COLOR RAYWHITE

// Camera 
#define DEFAULT_ZOOM 0.9f
#define CAM_MOV_RATE 5.0f
#define ZOOM_RATE 0.1f
#define ZOOM_MIN 0.75f
#define ZOOM_MAX 10.0f


// PROTOTYPES
void InitializeGrid(bool _alive);                       
void DrawGameGrid(void);       
void IlluminateGrid(void);
unsigned int GetNeighbors(size_t x, size_t y);
void UpdateGrid(void);
void RandomizeGrid(float probability);
void ClearGrid(void);
float randf(void);   
size_t GetPopulation(void);
void HandleInput(Camera2D cam, float interval, float *timeSinceLastUpdate);     
void HandleCamera(Camera2D *cam);                  

// PROGRAM VARIABLES
static Cell Grid[GRID_WIDTH][GRID_HEIGHT];
static Cell NextGrid[GRID_WIDTH][GRID_HEIGHT];
bool paused = true;
Vector2 middle = (Vector2){WWIDTH / 2, WHEIGHT / 2};

int main(void)
{   
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(WWIDTH, WHEIGHT, "CONWAY`S GAME OF LIFE");
    SetTraceLogLevel(LOG_FATAL);
    SetTargetFPS(60);

    srand(time(NULL));  // Seed random number generator once
    
    // Camera 
    Camera2D cam = {0};
    cam.offset = middle;
    cam.target = middle;
    cam.zoom = DEFAULT_ZOOM;

    float updateInterval = 0.05f;
    float timeSinceLastUpdate = 0.0f;
    InitializeGrid(false);
    RandomizeGrid(0.5);

    while (!WindowShouldClose())
    {   
        float deltaTime = GetFrameTime();
        timeSinceLastUpdate += deltaTime;

        HandleInput(cam, updateInterval, &timeSinceLastUpdate);  
        HandleCamera(&cam);

        BeginDrawing();
            BeginMode2D(cam);
                ClearBackground(BG_COLOR);
                DrawGameGrid();
                IlluminateGrid();
            EndMode2D();

            DrawFPS(10, 10);
            DrawText(TextFormat("%s", paused ? "PAUSED" : "RUNNING"), 10, 30, 20, GREEN);
            DrawText(TextFormat("Population: %d", GetPopulation()), 10, 50, 20, GREEN);
        EndDrawing();   
    }
    CloseWindow();
    return 0;
}

void HandleInput(Camera2D cam, float interval, float *timeSinceLastUpdate)
{   
    if (IsKeyPressed(KEY_R))                                                // Randomize Grid
    {
        InitializeGrid(false);
        RandomizeGrid(0.1);
    }

    if (IsKeyPressed(KEY_C))                    ClearGrid();                // Erase Grid
    if (IsKeyPressed(KEY_SPACE))                paused = !paused;           // toggle pause


    // Handle mouse input
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))                            // Select a cell 
    {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), cam);
        size_t x = mousePos.x / CELL_SIZE;
        size_t y = mousePos.y / CELL_SIZE;
        if (x < GRID_WIDTH && y < GRID_HEIGHT)
        {
            Grid[x][y].alive = !Grid[x][y].alive;
        }
    }

    // Update grid if not paused and interval has passed
    if (!paused && *timeSinceLastUpdate >= interval)
    {
        UpdateGrid();
        *timeSinceLastUpdate = 0.0f;
    }
}

void HandleCamera(Camera2D *cam)
{
    // Reset cam position to default target  
    if (IsKeyPressed(KEY_Z))
    {
        cam->target = middle;
        cam->offset = middle;
        cam->rotation = 0.0f;
    }

    // Reset cam position to default target & zoom 
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_Z))
    {
        cam->target = middle;
        cam->offset = middle;
        cam->zoom = 1.0f;
        cam->rotation = 0.0f;
    }

    // Camera moving around
    if (IsKeyDown(KEY_RIGHT))                   cam->target.x += CAM_MOV_RATE;
    if (IsKeyDown(KEY_LEFT))                    cam->target.x -= CAM_MOV_RATE;
    if (IsKeyDown(KEY_DOWN))                    cam->target.y += CAM_MOV_RATE;
    if (IsKeyDown(KEY_UP))                      cam->target.y -= CAM_MOV_RATE;

    // Camera zoom 
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f)
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *cam);
        cam->offset = GetMousePosition();
        cam->target = mouseWorldPos;

        cam->zoom += wheel * ZOOM_RATE * cam->zoom;
        cam->zoom = Clamp(cam->zoom, ZOOM_MIN, ZOOM_MAX);
    }

    // Camera Drag
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {   
        Vector2 mouseDelta = GetMouseDelta();
        Vector2 delta = Vector2Scale(mouseDelta, -1.0f / cam->zoom);
        cam->target = Vector2Add(cam->target, delta);
        
        cam->target.x = Clamp(cam->target.x, 0, GRID_WIDTH * CELL_SIZE * 0.75);
        cam->target.y = Clamp(cam->target.y, 0, GRID_HEIGHT * CELL_SIZE * 0.75);
    }
}


void InitializeGrid(bool _alive)
{   
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {
            Grid[i][j].alive = _alive;
            Grid[i][j].pos = (Vector2){i * CELL_SIZE, j * CELL_SIZE};
            NextGrid[i][j] = Grid[i][j];
        }
    }
}

void DrawGameGrid(void)
{
    for (size_t i = 0; i <= GRID_WIDTH; ++i)
    {
        DrawLineEx((Vector2){i * CELL_SIZE, 0}, 
        (Vector2){i * CELL_SIZE, WHEIGHT}, GRID_LINE_THICKNESS, GRID_LINE_COLOR);
    }
    for (size_t j = 0; j <= GRID_HEIGHT; ++j)
    {
        DrawLineEx((Vector2){0, j * CELL_SIZE}, 
        (Vector2){WWIDTH, j * CELL_SIZE}, GRID_LINE_THICKNESS, GRID_LINE_COLOR);
    }
    
    // Drawing the Axes lines for reference 
    DrawLineEx((Vector2){0, WHEIGHT / 2}, (Vector2){WWIDTH, WHEIGHT / 2}, GRID_AXIS_THICKNESS, GRID_AXIS_COLOR);   
    DrawLineEx((Vector2){WWIDTH / 2, 0}, (Vector2){WWIDTH / 2, WHEIGHT}, GRID_AXIS_THICKNESS, GRID_AXIS_COLOR);   
}

unsigned int GetNeighbors(size_t x, size_t y)
{   
    unsigned int aliveNeighbors = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0) continue;         // current cell skip
            size_t nx = (x + i + GRID_WIDTH) % GRID_WIDTH;
            size_t ny = (y + j + GRID_HEIGHT) % GRID_HEIGHT;
            aliveNeighbors += Grid[nx][ny].alive ? 1 : 0;
        }
    }
    return aliveNeighbors;
}

void UpdateGrid(void)
{
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {   
            unsigned int aliveNeighbors = GetNeighbors(i, j);

            if (Grid[i][j].alive) {
                // Rule for live cells:
                // Underpopulation: A live cell with fewer than 2 live neighbors dies.
                // Overpopulation: A live cell with more than 3 live neighbors dies.
                NextGrid[i][j].alive = (aliveNeighbors == 2 || aliveNeighbors == 3);
            } else {
                // Rule for dead cells:
                // Reproduction: A dead cell with exactly 3 live neighbors becomes alive.
                NextGrid[i][j].alive = (aliveNeighbors == 3);
            }
        }
    }

    // Copy NextGrid to Grid
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {
            Grid[i][j] = NextGrid[i][j];
        }
    }
}

float randf(void)
{
    return (float) rand() / (float) RAND_MAX;
}

void RandomizeGrid(float probability)
{   
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {
            Grid[i][j].alive = (randf() < probability);
        }
    }
}

size_t GetPopulation(void)
{   
    size_t population = 0;
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {
            if (Grid[i][j].alive) population++;
        }
    }
    return population;
}

void IlluminateGrid(void)
{
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {
            if (Grid[i][j].alive)
            {
                DrawRectangleV(Grid[i][j].pos, (Vector2){CELL_SIZE, CELL_SIZE}, WHITE);
            }
        }
    }
}

void ClearGrid(void)
{
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {
            Grid[i][j].alive = false;
        }
    }
}
