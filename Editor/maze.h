#ifndef MAZE_H
#define MAZE_H

#include <SDL2/SDL.h>

typedef struct
{
    char name[64];
    int size;
    SDL_Texture* mazeTexture;
    int hWalls [17][16];
    int vWalls [16][17];
} Maze;

int loadMaze(Maze* maze, const char *path);
int updateWall(Maze *maze, int mx, int my);
int renderMaze(Maze *maze);
int saveMaze(Maze *maze, const char *path);

#endif