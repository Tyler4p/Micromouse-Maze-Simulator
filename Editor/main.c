#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

// Global variables
// NOTE: not all constant; some of these get updated if window is resized
const int INIT_WIDTH = 640;         // Initial screen width (pixels)
const int INIT_HEIGHT = 480;        // Initial screen height (pixels)
int SCREEN_WIDTH = INIT_WIDTH;      // Screen width (pixels)
int SCREEN_HEIGHT = INIT_HEIGHT;    // Screen height (pixels)
int MAZE_PADDING = 50;              // Padding between maze border and edge of screen (pixels)
int WALL_THICKNESS = 10;            // Thickness of maze wall (pixels)
int MAX_MAZE_SIZE = 16;             // Maximum maze dimensions (cells)
int MIN_MAZE_SIZE = 2;              // Minimum maze dimensions (cells)
int MAZE_WIDTH;                     // Width of maze border wall (pixels)
int CELL_LENGTH;                    // Length of one cell (pixels)
int x_0, y_0;                       // Position of upper left maze corner (pixels)
int MIN_SCREEN_DIMENSION;           // Store the smaller screen dimension out of the width and height (pixels)
int MAX_SCREEN_DIMENSION;           // Store the larger screen dimension out of the width and height (pixels)
char mazePath[64];                  // Name of default maze file used if no maze path provided
char *mazeDirectory = "../Mazes/";  // Path to maze folder
int SELECTION_PADDING = 10;         // Border around walls to check for clicks

// Global structs/objects
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event e;
Maze maze;

// Function prototypes
int init();
int resizeWindow(int width, int height);
int quit();
void renderScreen();
//void renderText();

// MAIN
// Can take in one parameter that is the path to a maze file
int main(int argc, char** argv)
{
    // Initialize maze
    if (argc == 2) strcpy(mazePath, argv[1]);
    else
    {
        printf("Enter name of maze file to edit or create: ");
        scanf("%[^\n]%*c", mazePath); 
    }

    if(!loadMaze(&maze, mazePath))
    {
        printf("Error initializing maze, exiting\n");
        return quit() - 1;
    }

    // Initialize application
    if(!init()) return -1;

    CELL_LENGTH = MAZE_WIDTH / maze.size;

    // Render initial maze to screen
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

                // Display resized maze
                renderScreen();
            }
            
            else if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                updateWall(&maze, mx, my);

                // Display updated maze
                renderScreen();
            }

            else if(e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_s:
                        printf("Saving file %s...\n", mazePath);
                        if(!saveMaze(&maze, mazePath)) printf("Error: unable to save maze\n");
                        else printf("Maze saved successfully\n");
                        break;
                }
            }
        }
    }

    return 0;
}

// Renders maze to screen
void renderScreen()
{
    // Prepare new render frame
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // Render maze to frame
    renderMaze(&maze);

    // For future implementation :)
    //renderText();

    // Display render frame to screen
    SDL_RenderPresent(renderer);
}

// void renderText()
// {
//     TTF_Font *font = TTF_OpenFont("OpenSans-Light.ttf", 28);
//     SDL_Color textColor = {255, 0, 0};
//     SDL_Surface *textSurface = TTF_RenderText_Solid(font, "testText", textColor);
//     SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
//     SDL_Rect textBox = {0, 0, textSurface->w, textSurface->h};
//     SDL_RenderCopy(renderer, textTexture, NULL, &textBox);

//     SDL_FreeSurface(textSurface);
// }

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
    window = SDL_CreateWindow("Maze Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    // if(TTF_Init() == -1)
    // {
    //     printf("Error initializing SDL_ttf.! SDL_ttf Error: %s\n", TTF_GetError());
    //     return 0;
    // }

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