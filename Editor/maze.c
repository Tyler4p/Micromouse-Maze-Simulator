#include "maze.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Access global variables from main.c
extern int SCREEN_WIDTH, SCREEN_HEIGHT, MAZE_PADDING, WALL_THICKNESS, MAX_MAZE_SIZE, MIN_MAZE_SIZE, MIN_SCREEN_DIMENSION, MAX_SCREEN_DIMENSION, MAZE_WIDTH, CELL_LENGTH, x_0, y_0;
extern SDL_Renderer* renderer;
extern int SELECTION_PADDING;
extern char *mazeDirectory;

// Load maze from specified path if path is provided
int loadMaze(Maze *maze, const char *path)
{
    // If maze pointer passed in was NULL, exit
    if(maze == NULL) return 0;

    // File loading variables
    FILE *file;
    char *buffer = (char *) malloc(64);
    char *filePath[100] = {0};
    int lineLength = 0;
    int mazeSize = 0;

    // Load specified maze file or default one if NULL path provided
    strcat(filePath, mazeDirectory);
    if(path) strcat(filePath, path);
    else strcat(filePath, "default.txt");

    // Open maze file
    file = fopen(filePath, "r");

    // Check if file exists
    if(access(filePath, F_OK) == 0)
    {

        if(!file)
        {
            printf("Error: Could not open file %s\n", filePath);
            return 0;
        }
    }

    // File does not exist, create new blank maze file
    else
    {
        printf("Maze file does not exist, creating new maze file\n");
        printf("Enter a name for the maze: ");
        scanf("%[^\n]%*c", buffer);
        strcpy(maze->name, buffer);

        do
        {
            printf("Enter a maze size: ");
            scanf("%d", &mazeSize);

            if(mazeSize < 2 || mazeSize > 16) printf("Error: maze size must be between 2 and 16\n");

        } while (mazeSize < 2 || mazeSize > 16);

        maze->size = mazeSize;
        maze->mazeTexture = NULL;

        // Set maze wall arrays to all zeros
        int h_walls[17][16] = {0};
        int v_walls[16][17] = {0};
        memcpy(maze->hWalls, h_walls, sizeof(int) * 17 * 16);
        memcpy(maze->vWalls, v_walls, sizeof(int) * 17 * 16);

        return 1;
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

int saveMaze(Maze *maze, const char *path)
{
    // If maze pointer passed in was NULL, exit
    if(maze == NULL) return 0;

    // File loading variables
    FILE *file;
    char *filePath[100] = {0};
    int mazeSize = maze->size;
    int hCounter;
    int vCounter;

    // Load specified maze file or default one if NULL path provided
    strcat(filePath, mazeDirectory);
    strcat(filePath, path);

    // Open maze file
    file = fopen(filePath, "w+");

    if(!file)
    {
        printf("Error: Could not open or create file %s\n", filePath);
        return 0;
    }

    fprintf(file, "%s\n%d\n", maze->name, mazeSize);
    
    // Create top border wall
    for(int i = 0; i < 2*mazeSize+1; i++)
    {
        fprintf(file, "x");
    }
    fprintf(file, "\n");

    // Process two lines at a time up to second-to-last line and detect horizontal and vertical walls
    for(int i = 0; i < mazeSize - 1; i++)
    {        
        vCounter = 0;
        fprintf(file, "x");
        for(int j = 2; j <= 2*mazeSize - 1; j += 2)
        {
            fprintf(file, " ");
            if(maze->vWalls[i][vCounter] == 1) fprintf(file, "x");
            else fprintf(file, " ");
            vCounter++;
        }
        fprintf(file, " x\n");

        hCounter = 0;
        fprintf(file, "x");
        for(int k = 1; k <= 2*mazeSize + 1; k += 2)
        {
            if(k == 2*mazeSize + 1)
            {
                if(maze->hWalls[i][hCounter] == 1) fprintf(file, "xx\n");
                else fprintf(file, " x\n");
            }
            else
            {
                if(maze->hWalls[i][hCounter] == 1) fprintf(file, "xx");
                else fprintf(file, "  ");
            }
            
            hCounter++;
        }
    }

    // Process last line (bottom maze row with vertical walls)
    vCounter = 0;
    fprintf(file, "x");
    for(int i = 2; i <= 2*mazeSize - 1; i += 2)
    {
        fprintf(file, " ");
        if(maze->vWalls[mazeSize - 1][vCounter] == 1) fprintf(file, "x");
        else fprintf(file, " ");
        vCounter++;
    }
    fprintf(file, " x\n");

    // Create bottom border wall
    for(int i = 0; i < 2*mazeSize+1; i++)
    {
        fprintf(file, "x");
    }
    fprintf(file, "\n");

    // Close file
    fclose(file);

    return 1;
}

// Render maze to screen
int renderMaze(Maze *maze)
{
    // Draw white background
    SDL_Rect tempRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);    
    SDL_RenderFillRect(renderer, &tempRect);

    // Create border walls
    SDL_Rect mazeBorders[4] = {
        {x_0, y_0, (CELL_LENGTH * maze->size) + WALL_THICKNESS, WALL_THICKNESS},
        {x_0, y_0, WALL_THICKNESS, (CELL_LENGTH * maze->size) + WALL_THICKNESS},
        {x_0 + (CELL_LENGTH * maze->size), y_0, WALL_THICKNESS, (CELL_LENGTH * maze->size) + WALL_THICKNESS},
        {x_0, y_0 + (CELL_LENGTH * maze->size), (CELL_LENGTH * maze->size) + WALL_THICKNESS, WALL_THICKNESS}
    };

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderFillRects(renderer, mazeBorders, 4);

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
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);    
                SDL_RenderFillRect(renderer, &tempRect);
            }
            else
            {
                SDL_Rect tempRect = {x, y, WALL_THICKNESS, CELL_LENGTH + WALL_THICKNESS};
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);    
                SDL_RenderDrawRect(renderer, &tempRect);
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
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);  
                SDL_RenderFillRect(renderer, &tempRect);
            }
            else
            {
                SDL_Rect tempRect = {x, y, CELL_LENGTH + WALL_THICKNESS, WALL_THICKNESS};
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);  
                SDL_RenderDrawRect(renderer, &tempRect);
            }

            x += CELL_LENGTH;
        }

        y += CELL_LENGTH;
    }

    return 1;
}

// Checks if mouse click coincides with a wall and toggles it if it does
int updateWall(Maze *maze, int mx, int my)
{
    // Render vertical walls
    int x = x_0 + CELL_LENGTH;
    int y = y_0;
    for(int i = 0; i < maze->size; i++)
    {
        x = x_0 + CELL_LENGTH;

        for(int j = 0; j < maze->size - 1; j++)
        {
            if(mx >= x - SELECTION_PADDING && mx <= x + WALL_THICKNESS + SELECTION_PADDING && my > y && my < y + CELL_LENGTH + WALL_THICKNESS)
            {
                maze->vWalls[i][j] = !(maze->vWalls[i][j]);
                return 1;
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
            if(mx >= x && mx <= x + CELL_LENGTH + WALL_THICKNESS && my > y - SELECTION_PADDING && my < y + WALL_THICKNESS + SELECTION_PADDING)
            {
                maze->hWalls[i][j] = !(maze->hWalls[i][j]);
                return 1;
            }

            x += CELL_LENGTH;
        }

        y += CELL_LENGTH;
    }
}