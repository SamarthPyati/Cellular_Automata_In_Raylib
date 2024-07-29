/*
** A Simple Implementation of Conway`s game of life. **
    - Author: Samarth Sanjay Pyati
    - Date: 23-07-2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h" 

// WINDOW 
const size_t WWIDTH = 1440;
const size_t WHEIGHT = 900;

// #define BG_COLOR (Color) {39, 41, 40, 255}
#define BG_COLOR BLACK

// SIMULATION MODE 
typedef enum {
    CONWAY,
    BRIAN,
} GameMode;    

typedef enum {
    OFF,
    ON,
    DYING
} CellState;

// CELL
typedef struct 
{
    Vector2 pos;
    union {
        bool alive;  // For Conway's Game of Life
        CellState state;  // For Brian's Brain
    };
} Cell;


#define CELL_SIZE 5                 

// GRID
#define GRID_WIDTH (WWIDTH / CELL_SIZE)
#define GRID_HEIGHT (WHEIGHT / CELL_SIZE)
#define GRID_LINE_THICKNESS 0.2f
#define GRID_LINE_COLOR WHITE
#define GRID_AXIS_THICKNESS 0.9f
#define GRID_AXIS_COLOR WHITE

// CAMERA
#define DEFAULT_ZOOM 1.0f
#define CAM_MOV_RATE 5.0f
#define ZOOM_RATE 0.1f
#define ZOOM_MIN 0.75f
#define ZOOM_MAX 10.0f

// HELPER MACRO
#define ARRAY_LEN(xs) (sizeof(xs) / sizeof(xs[0]))


// PROTOTYPES
void InitializeGrid(bool _alive);                       
void DrawGameGrid(Camera2D *cam);       
void IlluminateGrid(void);
unsigned int GetNeighbors(size_t x, size_t y);
void UpdateGrid(void);
void RandomizeGrid(float probability);
void ClearGrid(void);
float randf(void);   
size_t GetPopulation(void);
void HandleInput(Camera2D cam, float interval, float *timeSinceLastUpdate);     
void HandleCamera(Camera2D *cam);     
void SwitchRuleSet(void);             

// PROGRAM VARIABLES
static Cell Grid[GRID_WIDTH][GRID_HEIGHT];
static Cell NextGrid[GRID_WIDTH][GRID_HEIGHT];
static GameMode currentGameMode = CONWAY;
bool paused = false;
bool drawGrid = true;
Vector2 middle = (Vector2){WWIDTH / 2, WHEIGHT / 2};

const char *musicPaths[3] = {
    "assets/epic-hollywood-trailer-9489.mp3",
    "assets/honor-and-sword-main-11222.mp3", 
    "assets/winning-elevation-111355.mp3"
};

unsigned int musicCount = 3;
unsigned int currentMusicIndex = 0;

int main(void)
{   
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(WWIDTH, WHEIGHT, "CONWAY`S GAME OF LIFE");
    InitAudioDevice();          
    SetTraceLogLevel(LOG_FATAL);
    SetTargetFPS(60);

    srand(time(NULL));  // Seed random number generator once
    
    // Camera Initialize and setup
    Camera2D cam = {0};
    cam.offset = middle;
    cam.target = middle;
    cam.zoom = DEFAULT_ZOOM;

    float updateInterval = 0.05f;
    float timeSinceLastUpdate = 0.0f;
    const char *currentMusicPath = musicPaths[currentMusicIndex % musicCount];
    
    InitializeGrid(false);
    RandomizeGrid(0.5);
    Music music = LoadMusicStream(currentMusicPath);
    PlayMusicStream(music);

    while (!WindowShouldClose())
    {   
        float deltaTime = GetFrameTime();
        timeSinceLastUpdate += deltaTime;

        HandleInput(cam, updateInterval, &timeSinceLastUpdate);  
        HandleCamera(&cam);

        UpdateMusicStream(music);
        
        // Pause/Resume music playing
        if (IsKeyPressed(KEY_SPACE))
        {
            if (paused) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }

        if (GetMusicTimePlayed(music) >= GetMusicTimeLength(music) - 0.1f)  // Use a small threshold
        {
            currentMusicIndex = (currentMusicIndex + 1) % musicCount;
            UnloadMusicStream(music);
            music = LoadMusicStream(musicPaths[currentMusicIndex]);
            PlayMusicStream(music);
        }

        BeginDrawing();
            ClearBackground(BG_COLOR);
            BeginMode2D(cam);
                DrawGameGrid(&cam);    
                IlluminateGrid();
            EndMode2D();

            DrawFPS(10, 10);
            DrawText(TextFormat("%s", paused ? "PAUSED" : "RUNNING"), 10, 30, 20, GREEN);
            DrawText(TextFormat("Population: %d", GetPopulation()), 10, 50, 20, GREEN);
        EndDrawing();   
    }
    
    UnloadMusicStream(music);
    CloseAudioDevice();
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
    if (IsKeyPressed(KEY_G))                    drawGrid = !drawGrid;
    if (IsKeyPressed(KEY_B))                    SwitchRuleSet();

    // Handle mouse input
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))                            // Select a cell 
    {
        Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), cam);
        size_t x = mousePos.x / CELL_SIZE;
        size_t y = mousePos.y / CELL_SIZE;
        if (x < GRID_WIDTH && y < GRID_HEIGHT)
        {
            if (currentGameMode == CONWAY)
                Grid[x][y].alive = !Grid[x][y].alive;
            else
                Grid[x][y].state = (Grid[x][y].state == OFF) ? ON : OFF;
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
            if (currentGameMode == CONWAY)
                Grid[i][j].alive = _alive;
            else
                Grid[i][j].state = OFF;
            
            Grid[i][j].pos = (Vector2){i * CELL_SIZE, j * CELL_SIZE};
            NextGrid[i][j] = Grid[i][j];
        }
    }
}

void DrawGameGrid(Camera2D *cam)
{
    for (size_t i = 0; i <= GRID_WIDTH; ++i)
    {   
        Vector2 startPos = GetWorldToScreen2D((Vector2){i * CELL_SIZE, 0}, *cam);
        Vector2 endPos = GetScreenToWorld2D((Vector2){i * CELL_SIZE, WHEIGHT}, *cam);
        DrawLineEx(startPos, endPos, GRID_LINE_THICKNESS, GRID_LINE_COLOR);
    }
    for (size_t j = 0; j <= GRID_HEIGHT; ++j)
    {   
        Vector2 startPos = GetScreenToWorld2D((Vector2){0, j * CELL_SIZE}, *cam);
        Vector2 endPos = GetScreenToWorld2D((Vector2){WWIDTH, j * CELL_SIZE}, *cam);
        DrawLineEx(startPos, endPos, GRID_LINE_THICKNESS, GRID_LINE_COLOR);
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
            if (i == 0 && j == 0) continue;
            size_t nx = (x + i + GRID_WIDTH) % GRID_WIDTH;
            size_t ny = (y + j + GRID_HEIGHT) % GRID_HEIGHT;
            if (currentGameMode == CONWAY)
                aliveNeighbors += Grid[nx][ny].alive ? 1 : 0;
            else
                aliveNeighbors += (Grid[nx][ny].state == ON) ? 1 : 0;
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

            // Conway`s Game of life
            if (currentGameMode == CONWAY)
            {
                if (Grid[i][j].alive) {
                    NextGrid[i][j].alive = (aliveNeighbors == 2 || aliveNeighbors == 3);
                } else {
                    NextGrid[i][j].alive = (aliveNeighbors == 3);
                }
            }
            else  // Brian's Brain
            {
                switch(Grid[i][j].state)
                {
                    case OFF:
                        NextGrid[i][j].state = (aliveNeighbors == 2) ? ON : OFF;
                        break;
                    case ON:
                        NextGrid[i][j].state = DYING;
                        break;
                    case DYING:
                        NextGrid[i][j].state = OFF;
                        break;
                }
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
            float r = randf();
            if (currentGameMode == CONWAY)
            {
                Grid[i][j].alive = (r < probability);
            }
            else  // Brian's Brain
            {
                if (r < probability / 2)
                    Grid[i][j].state = ON;
                else if (r < probability)
                    Grid[i][j].state = DYING;
                else
                    Grid[i][j].state = OFF;
            }
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
            if (currentGameMode == CONWAY)
            {
                if (Grid[i][j].alive) population++;
            }
            else  // Brian's Brain
            {
                if (Grid[i][j].state == ON) population++;
            }
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
            Color cellColor;
            if (currentGameMode == CONWAY)
            {
                cellColor = Grid[i][j].alive ? RAYWHITE : BG_COLOR;
            }
            else  // Brian's Brain
            {
                switch(Grid[i][j].state)
                {
                    case ON:
                        cellColor = RAYWHITE;
                        break;
                    case DYING:
                        cellColor = RED;
                        break;
                    case OFF:
                        cellColor = BG_COLOR;
                        break;
                }
            }
            DrawRectangleV(Grid[i][j].pos, (Vector2){CELL_SIZE, CELL_SIZE}, cellColor);
        }
    }
}

void ClearGrid(void)
{
    for (size_t i = 0; i < GRID_WIDTH; ++i)
    {
        for (size_t j = 0; j < GRID_HEIGHT; ++j)
        {
            if (currentGameMode == CONWAY)
                Grid[i][j].alive = false;
            else    
                Grid[i][j].state = OFF;
        }
    }
}

void SwitchRuleSet(void)
{
    currentGameMode = (currentGameMode == CONWAY) ? BRIAN : CONWAY;
    InitializeGrid(false);
    RandomizeGrid(0.1);
}