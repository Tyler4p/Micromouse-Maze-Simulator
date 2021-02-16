#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "mouse.h"
#include "maze.h"
#include <math.h>
#include "solver.h"


extern int SCREEN_WIDTH, SCREEN_HEIGHT, MAZE_PADDING, WALL_THICKNESS, MAX_MAZE_SIZE, MIN_MAZE_SIZE, MIN_SCREEN_DIMENSION, MAX_SCREEN_DIMENSION, MAZE_WIDTH, CELL_LENGTH, x_0, y_0;
extern float MOUSE_SIZE, xCorrection, yCorrection;
extern int MOVE_STEP, TURN_STEP;
extern SDL_Renderer* renderer;
extern Maze maze;
extern int running;

// Store goal and initial position and angle offsets; used for rendering
int goalDx = 0, goalDy = 0, goalDa = 0, xi = 0, yi = 0, ai = 0;

int loadMouse(Mouse* mouse)
{
    // Initialize mouse position and heading
    mouse->x = 0;
    mouse->y = 0;
    mouse->absX = 0;
    mouse->absY = 0;
    mouse->absA = 0;
    mouse->heading = NORTH;
    mouse->action = IDLE;
    mouse->maze = &maze;

    // Load mouse image
    SDL_Surface* tempSurface = IMG_Load("./mouse.png");
    if(tempSurface == NULL)
    {
        printf( "Error: Unable to load image %s! SDL_image Error: %s\n", "./mouse.png", IMG_GetError() );
        exit(-1);
    }

    mouse->mouseTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Initialize mouseRect and mouseCenter
    updateRect(mouse);

    return 1;
}


// Returns direction mouse is facing
Heading getHeading(Mouse* mouse)
{
    return mouse->heading;
}

// Gets next mouse action from solver algorithm and then performs the action
Action getNextAction(Mouse* mouse)
{
    Action action = solver(mouse);
    mouse->action = action;

    // Handles each action
    switch(action)
    {
        // Update position and goal movement depending on heading
        case FORWARD:
            if(getFrontReading(mouse))
            {
                printf("Error: mouse attempted to move through wall\nPress s to resume\n");
                running = 0;
                return IDLE;
            }
            else
            {
                switch(getHeading(mouse))
                {
                    case NORTH:
                        mouse->y++;
                        goalDy = CELL_LENGTH / yCorrection;
                        break;

                    case EAST:
                        mouse->x++;
                        goalDx = CELL_LENGTH / xCorrection;
                        break;

                    case SOUTH:
                        mouse->y--;
                        goalDy = -CELL_LENGTH / yCorrection;
                        break;

                    case WEST:
                        mouse->x--;
                        goalDx = -CELL_LENGTH / xCorrection;
                        break;
                }
                break;
            }

        // Update heading and set goal rotation amount
        case LEFT:
            switch(mouse->heading)
            {
                case NORTH:
                    mouse->heading = WEST;
                    break;

                case EAST:
                    mouse->heading = NORTH;
                    break;

                case SOUTH:
                    mouse->heading = EAST;
                    break;

                case WEST:
                    mouse->heading = SOUTH;
                    break;
            }

            goalDa = -90;
            break;

        // Update heading and set goal rotation amount
        case RIGHT:
            switch(mouse->heading)
            {
                case NORTH:
                    mouse->heading = EAST;
                    break;

                case EAST:
                    mouse->heading = SOUTH;
                    break;

                case SOUTH:
                    mouse->heading = WEST;
                    break;

                case WEST:
                    mouse->heading = NORTH;
                    break;
            }
            goalDa = 90;
            break;

        // Do nothing
        case IDLE:
            break;
    }

    return action;
}


// Handles rendering of mouse moves and turns
// Return 0 if rendering of a movement is finished or if mouse is idling
// Return 1 if a movement is currently being rendered
int renderMouse(Mouse *mouse)
{
    // Renders mouse to screen
    SDL_RenderCopyEx(renderer, mouse->mouseTexture, NULL, &(mouse->mouseRect), mouse->absA, &(mouse->mouseCenter), SDL_FLIP_NONE);

    // Update mouse texture bounding box
    updateRect(mouse);

    if(!running) return IDLE;

    // dx, dy, and da store offset amounts
    int dx = 0, dy = 0, da = 0;

    // Set offsets depending on action and current heading
    switch(mouse->action)
    {
        case FORWARD:
            switch(mouse->heading)
            {
                case NORTH:
                    dy = MOVE_STEP;
                    break;

                case EAST:
                    dx = MOVE_STEP;
                    break;

                case SOUTH:
                    dy = -MOVE_STEP;
                    break;

                case WEST:
                    dx = -MOVE_STEP;
                    break;
            }
            break;

        case LEFT:
            da = -TURN_STEP;
            break;

        case RIGHT:
            da = TURN_STEP;
            break;

        case IDLE:
            break;
    }

    // If movement not complete, add offsets to current position
    if((goalDx < 0 && xi > goalDx) || (goalDx > 0 && xi < goalDx) || 
       (goalDy < 0 && yi > goalDy) || (goalDy > 0 && yi < goalDy) ||
       (goalDa < 0 && ai > goalDa) || (goalDa > 0 && ai < goalDa))
    {
        int correction = xCorrection*yCorrection;
        xi += dx * correction;
        yi += dy * correction;
        ai += da * correction;

        mouse->absX += dx * correction;
        mouse->absY += dy * correction;
        mouse->absA += da * correction;

        return 1;
    }
    else
    {
        // Snap mouse to grid (in case it moved an uneven amount)
        mouse->absX = CELL_LENGTH * mouse->x / xCorrection;
        mouse->absY = CELL_LENGTH * mouse->y / yCorrection;
        mouse->absA = round(mouse->absA / 90) * 90;

        // Reset goal offsets and initial offsets
        goalDx = goalDy = goalDa = xi = yi = ai = 0;

        return 0;
    }
}

// Updates mouse texture bounding box
void updateRect(Mouse *mouse)
{
    int padding = CELL_LENGTH - 2*WALL_THICKNESS - (CELL_LENGTH - 2*WALL_THICKNESS) * MOUSE_SIZE;
    mouse->mouseRect.x = x_0 + WALL_THICKNESS + padding + mouse->absX * xCorrection;
    mouse->mouseRect.y = y_0 + MAZE_WIDTH - CELL_LENGTH - WALL_THICKNESS + padding - mouse->absY * yCorrection;
    mouse->mouseRect.w = CELL_LENGTH - 2*WALL_THICKNESS - 2*padding;
    mouse->mouseRect.h = CELL_LENGTH - 2*WALL_THICKNESS - 2*padding;

    mouse->mouseCenter.x = mouse->mouseRect.w/2;
    mouse->mouseCenter.y = mouse->mouseRect.h * 0.711;
}

// Returns 1 if there is a wall to the mouse's left, 0 otherwise
int getLeftReading(Mouse *mouse)
{
    Heading heading = mouse->heading;
    Maze* maze = mouse->maze;
    int size = maze->size - 1;

    switch(heading)
    {
        case NORTH:
            return (mouse->x == 0) ? 1 : maze->vWalls[size - mouse->y][mouse->x - 1];
            break;

        case EAST:
            return (mouse->y == size) ? 1 : maze->hWalls[size - 1 - mouse->y][mouse->x];
            break;

        case SOUTH:
            return (mouse->x == size) ? 1 : maze->vWalls[size - mouse->y][mouse->x];
            break;

        case WEST:
            return (mouse->y == 0) ? 1 : maze->hWalls[size - mouse->y][mouse->x];
            break;
    }
}

// Returns 1 if there is a wall in front of the mouse, 0 otherwise
int getFrontReading(Mouse *mouse)
{
    Heading heading = mouse->heading;
    Maze* maze = mouse->maze;
    int size = maze->size - 1;

    switch(heading)
    {
        case NORTH:
            return (mouse->y == size) ? 1 : maze->hWalls[size - 1 - mouse->y][mouse->x];
            break;

        case EAST:
            return (mouse->x == size) ? 1 : maze->vWalls[size - mouse->y][mouse->x];
            break;

        case SOUTH:
            return (mouse->y == 0) ? 1 : maze->hWalls[size - mouse->y][mouse->x];
            break;

        case WEST:
            return (mouse->x == 0) ? 1 : maze->vWalls[size - mouse->y][mouse->x - 1];
            break;
    }
}

// Returns 1 if there is a wall to the mouse's right, 0 otherwise
int getRightReading(Mouse *mouse)
{
    Heading heading = mouse->heading;
    Maze* maze = mouse->maze;
    int size = maze->size - 1;

    switch(heading)
    {
        case NORTH:
            return (mouse->x == size) ? 1 : maze->vWalls[size - mouse->y][mouse->x];
            break;

        case EAST:
            return (mouse->y == 0) ? 1 : maze->hWalls[size - mouse->y][mouse->x];
            break;

        case SOUTH:
            return (mouse->x == 0) ? 1 : maze->vWalls[size - mouse->y][mouse->x - 1];
            break;

        case WEST:
            return (mouse->y == size) ? 1 : maze->hWalls[size - 1 - mouse->y][mouse->x];
            break;
    }
}
