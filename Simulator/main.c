#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "mouse.h"

// Global variables
// NOTE: not all constant; some of these get updated if window is resized
const int INIT_WIDTH = 640;         // Initial screen width (pixels)
const int INIT_HEIGHT = 480;        // Initial screen height (pixels)
int SCREEN_WIDTH = INIT_WIDTH;      // Screen width (pixels)
int SCREEN_HEIGHT = INIT_HEIGHT;    // Screen height (pixels)
int MAZE_PADDING = 50;              // Padding between maze border and edge of screen (pixels)
int WALL_THICKNESS = 5;             // Thickness of maze wall (pixels)
int MAX_MAZE_SIZE = 16;             // Maximum maze dimensions (cells)
int MIN_MAZE_SIZE = 2;              // Minimum maze dimensions (cells)
float MOUSE_SIZE = 0.75;            // Size of mouse (percentage of cell size)
int MOVE_STEP = 2;                  // Distance to move in single frame (pixels)
int TURN_STEP = 3;                  // Amount to turn in single frame (degrees)
float xCorrection = 1;              // Scaling factor for when screen is resized
float yCorrection = 1;              // Scaling factor for when screen is resized
int MAZE_WIDTH;                     // Width of maze border wall (pixels)
int CELL_LENGTH;                    // Length of one cell (pixels)
int x_0, y_0;                       // Position of upper left maze corner (pixels)
int MIN_SCREEN_DIMENSION;           // Store the smaller screen dimension out of the width and height (pixels)
int MAX_SCREEN_DIMENSION;           // Store the larger screen dimension out of the width and height (pixels)
char *mazePath = "default.txt";     // Name of default maze file used if no maze path provided
char *mazeDirectory = "../Mazes/";  // Path to maze folder
int running = 0;                    // 1 if actively running simulation, 0 otherwise

// Wall and background colors
SDL_Color BACKGROUND_COLOR = {255,255,255};
SDL_Color WALL_COLOR = {0,0,0};

// Global structs/objects
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event e;
Maze maze;
Mouse mouse;

// Function prototypes
int init();
int resizeWindow(int width, int height);
int quit();
void renderScreen();

// MAIN
// Can take in one parameter that is the path to a maze file
int main(int argc, char** argv)
{
    // Initialize application
    if(!init()) return -1;

    // Initialize maze
    if (argc == 2) mazePath = argv[1];
    if(!loadMaze(&maze, mazePath))
    {
        printf("Error initializing maze, exiting\n");
        return quit() - 1;
    }

    CELL_LENGTH = MAZE_WIDTH / maze.size;
    maze.mazeTexture = createMazeTexture(&maze);

    // Initialize mouse
    if(!loadMouse(&mouse))
    {
        printf("Error initializing mouse, exiting\n");
        return quit() - 1;
    }

    printf("Press s to start simulation\n");

    // Render initial screen
    renderScreen();

    // Program loop
    while(1)
    {
        // Process event queue
        while(SDL_PollEvent(&e) != 0)
        {
            // Quit if close button is pressed
            if(e.type == SDL_QUIT) return quit();

            // Re-render window and maze if window is resized
            else if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                // Calculate new maze size constants
                SCREEN_WIDTH = e.window.data1;
                SCREEN_HEIGHT = e.window.data2;
                MIN_SCREEN_DIMENSION = (SCREEN_WIDTH <= SCREEN_HEIGHT) ? SCREEN_WIDTH : SCREEN_HEIGHT;
                MAX_SCREEN_DIMENSION = (SCREEN_WIDTH <= SCREEN_HEIGHT) ? SCREEN_HEIGHT : SCREEN_WIDTH;
                MAZE_WIDTH = MIN_SCREEN_DIMENSION - 2*MAZE_PADDING;
                CELL_LENGTH = MAZE_WIDTH / maze.size;

                // Calculate new maze origin
                if(SCREEN_WIDTH >= SCREEN_HEIGHT)
                {
                    y_0 = MAZE_PADDING;
                    x_0 = (MAX_SCREEN_DIMENSION - MAZE_WIDTH)/2;
                }
                else
                {
                    x_0 = MAZE_PADDING;
                    y_0 = (MAX_SCREEN_DIMENSION - MAZE_WIDTH)/2; 
                }

                // Update scaling factors
                xCorrection = (float) SCREEN_WIDTH / (float) INIT_WIDTH;
                yCorrection = (float) SCREEN_HEIGHT / (float) INIT_HEIGHT;

                // Re-size maze texture and mouse bounding box
                maze.mazeTexture = createMazeTexture(&maze);
                updateRect(&mouse);

                // Display changes
                renderScreen();
            }

            // Handle key presses
            else if(e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    // S: toggle simulation
                    case SDLK_s:
                        running = !running;
                        break;

                    // D: Toggle darkmode
                    case SDLK_d:
                        BACKGROUND_COLOR.r = ~BACKGROUND_COLOR.r;
                        BACKGROUND_COLOR.g = ~BACKGROUND_COLOR.g;
                        BACKGROUND_COLOR.b = ~BACKGROUND_COLOR.b;
                        WALL_COLOR.r = ~WALL_COLOR.r;
                        WALL_COLOR.g = ~WALL_COLOR.g;
                        WALL_COLOR.b = ~WALL_COLOR.b;
                        maze.mazeTexture = createMazeTexture(&maze);
                        renderScreen();
                        break;

                    // I: Credits :)
                    case SDLK_i:
                        printf("\nMaze Simulator v1.0\nCreated by Tyler Price for IEEE@UCLA Micromouse\n\n");
                        break;

                    // ESC: Quit simulator
                    case SDLK_ESCAPE:
                        return quit();
                }
            }
        }

        if(running)
        {
            // Render... the screen
            renderScreen();

            // Animation delay
            SDL_Delay(5);
        }
    }

    return 0;
}

void renderScreen()
{
    // Prepare new render frame
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // Render maze to frame
    renderMaze(&maze);

    // Render mouse if it is in motion, otherwise poll solver for next mouse action
    if(!renderMouse(&mouse)) getNextAction(&mouse);

    // Display render frame to screen
    SDL_RenderPresent(renderer);
}

// Initialize graphics environment
int init()
{
    // Calculate maze constants
    MIN_SCREEN_DIMENSION = (SCREEN_WIDTH <= SCREEN_HEIGHT) ? SCREEN_WIDTH : SCREEN_HEIGHT;
    MAX_SCREEN_DIMENSION = (SCREEN_WIDTH <= SCREEN_HEIGHT) ? SCREEN_HEIGHT : SCREEN_WIDTH;
    MAZE_WIDTH = MIN_SCREEN_DIMENSION - 2*MAZE_PADDING;

    if(SCREEN_WIDTH >= SCREEN_HEIGHT)
    {
        y_0 = MAZE_PADDING;
        x_0 = (MAX_SCREEN_DIMENSION - MAZE_WIDTH)/2;
    }
    else
    {
        x_0 = MAZE_PADDING;
        y_0 = (MAX_SCREEN_DIMENSION - MAZE_WIDTH)/2; 
    }

    // Initialize SDL video module
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    
    // Initialize window
    window = SDL_CreateWindow("Maze Simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window == NULL)
    {
        printf("Error creating window: %s\n", SDL_GetError());
        return 0;
    }
    
    // Initialize renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return 0;
    }

    // Initialize SDL image
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("Error initializing SDL_image: %s\n", IMG_GetError());
        return 0;
    }

    return 1;
}

// Free up resources and quit SDL
int quit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    SDL_Quit();
    return 0;
}
