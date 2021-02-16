#include "maze.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Access global variables from main.c
extern int SCREEN_WIDTH, SCREEN_HEIGHT, MAZE_PADDING, WALL_THICKNESS, MAX_MAZE_SIZE, MIN_MAZE_SIZE, MIN_SCREEN_DIMENSION, MAX_SCREEN_DIMENSION, MAZE_WIDTH, CELL_LENGTH, x_0, y_0;
extern SDL_Renderer* renderer;
extern char *mazeDirectory;
extern SDL_Color BACKGROUND_COLOR, WALL_COLOR;

// Load maze from specified path if path is provided
int loadMaze(Maze *maze, const char *path)
{
    // If maze pointer passed in was NULL, exit
    if(maze == NULL) return 0;

    // File loading variables
    FILE *file;
    char *buffer = (char *) malloc(64);;
    char *filePath[100] = {0};
    int lineLength = 0;
    int mazeSize = 0;

    // Load specified maze file or default one if NULL path provided
    strcat(filePath, mazeDirectory);
    if(path) strcat(filePath, path);
    else strcat(filePath, "default.txt");

    // Open maze file
    file = fopen(filePath, "r");
    if(!file)
    {
        printf("Error: Could not open file %s\n", filePath);
        exit(-1);
    }
    
    printf("Successfully opened file %s\n", filePath);

    // Read maze name from file (1st line) and copy to maze's name array
    fscanf(file, "%[^\n]", buffer);
    strncpy(maze->name, buffer, 64);

    // Read maze size from file (2nd line) and copy to maze's size variable
    fscanf(file, "%d", &(maze->size));
    mazeSize = maze->size;

    // Ensure maze size is valid
    if(mazeSize < MIN_MAZE_SIZE || mazeSize > MAX_MAZE_SIZE)
    {
        printf("Error: Invalid maze size\n");
        fclose(file);
        return 0;
    }

    // Print maze name and size to terminal
    printf("Maze name: %s\n", maze->name);
    printf("Maze size: %d\n", maze->size);
    
    // Maze loading variables
    maze->mazeTexture = NULL;
    int h_walls[17][16] = {0};
    int v_walls[16][17] = {0};
    char *hBuff = (char *) malloc(34);
    char *vBuff = (char *) malloc(34);
    int hCounter;
    int vCounter;

    // Get rid of first line (top border wall)
    getline(&buffer, &lineLength, file);
    getline(&buffer, &lineLength, file);

    // Process two lines at a time up to second-to-last line and detect horizontal and vertical walls
    for(int i = 0; i < mazeSize - 1; i++)
    {
        getline(&vBuff, &lineLength, file);
        getline(&hBuff, &lineLength, file);
        
        vCounter = 0;
        for(int j = 2; j <= 2*mazeSize - 1; j += 2)
        {
            if(vBuff[j] == 'x') v_walls[i][vCounter] = 1;
            vCounter++;
        }

        hCounter = 0;
        for(int k = 1; k <= 2*mazeSize + 1; k += 2)
        {
            if(hBuff[k] == 'x') h_walls[i][hCounter] = 1;
            hCounter++;
        }
    }

    // Process last line (bottom maze row with vertical walls)
    getline(&vBuff, &lineLength, file);
    vCounter = 0;
    for(int i = 2; i <= 2*mazeSize - 1; i += 2)
    {
        if(vBuff[i] == 'x') v_walls[mazeSize - 1][vCounter] = 1;
        vCounter++;
    }

    // Free temporary buffers
    free(buffer);
    free(hBuff);
    free(vBuff);

    // Store wall data into maze's wall arrays
    memcpy(maze->hWalls, h_walls, sizeof(int) * 17 * 16);
    memcpy(maze->vWalls, v_walls, sizeof(int) * 17 * 16);

    // Close file
    fclose(file);

    return 1;
}

// Generate maze texture from wall arrays
SDL_Texture* createMazeTexture(Maze *maze)
{
    // Generate temporary blank rendering surface
    SDL_Surface* mazeSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

    unsigned int bgColor = SDL_MapRGB(mazeSurface->format, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
    unsigned int wallColor = SDL_MapRGB(mazeSurface->format, WALL_COLOR.r, WALL_COLOR.g, WALL_COLOR.b);

    // White background
    SDL_FillRect(mazeSurface, NULL, bgColor);

    // Create border walls
    SDL_Rect mazeBorders[4] = {
        {x_0, y_0, (CELL_LENGTH * maze->size) + WALL_THICKNESS, WALL_THICKNESS},
        {x_0, y_0, WALL_THICKNESS, (CELL_LENGTH * maze->size) + WALL_THICKNESS},
        {x_0 + (CELL_LENGTH * maze->size), y_0, WALL_THICKNESS, (CELL_LENGTH * maze->size) + WALL_THICKNESS},
        {x_0, y_0 + (CELL_LENGTH * maze->size), (CELL_LENGTH * maze->size) + WALL_THICKNESS, WALL_THICKNESS}
    };

    // Render vertical walls
    int x = x_0 + CELL_LENGTH;
    int y = y_0;
    for(int i = 0; i < maze->size; i++)
    {
        x = x_0 + CELL_LENGTH;

        for(int j = 0; j < maze->size - 1; j++)
        {
            if(maze->vWalls[i][j])
            {
                SDL_Rect tempRect = {x, y, WALL_THICKNESS, CELL_LENGTH + WALL_THICKNESS};
                SDL_FillRect(mazeSurface, &tempRect, wallColor);
            }

            x += CELL_LENGTH;
        }

        y += CELL_LENGTH;
    }

    // Render horizontal walls
    x = x_0;
    y = y_0 + CELL_LENGTH;
    for(int i = 0; i < maze->size - 1; i++)
    {
        x = x_0;

        for(int j = 0; j < maze->size; j++)
        {
            if(maze->hWalls[i][j])
            {
                SDL_Rect tempRect = {x, y, CELL_LENGTH + WALL_THICKNESS, WALL_THICKNESS};
                SDL_FillRect(mazeSurface, &tempRect, wallColor);
            }

            x += CELL_LENGTH;
        }

        y += CELL_LENGTH;
    }

    // Fill in wall rectangles and free temporary rendering surface
    SDL_FillRects(mazeSurface, mazeBorders, 4, wallColor);
    SDL_Texture* mazeTexture = SDL_CreateTextureFromSurface(renderer, mazeSurface);
    SDL_FreeSurface(mazeSurface);

    return mazeTexture;
}

// Renders maze to screen
int renderMaze(Maze *maze)
{
    SDL_RenderCopy(renderer, maze->mazeTexture, NULL, NULL);
    return 1;
}
