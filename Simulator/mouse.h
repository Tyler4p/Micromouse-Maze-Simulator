#ifndef MOUSE_H
#define MOUSE_H

#include <SDL2/SDL.h>
#include "maze.h"

// Encode headings and actions for readability
typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

typedef struct
{
    int x;
    int y;
    int absX;
    int absY;
    int absA;
    int heading;
    int action;
    Maze* maze;
    SDL_Texture* mouseTexture;
    SDL_Rect mouseRect;
    SDL_Point mouseCenter;
} Mouse;

int loadMouse(Mouse* mouse);
int getLeftReading(Mouse *mouse);
int getFrontReading(Mouse *mouse);
int getRightReading(Mouse *mouse);
void updateRect(Mouse *mouse);
int renderMouse(Mouse *mouse);
Heading getHeading(Mouse *mouse);
Action getNextAction(Mouse *mouse);

#endif
