#include "solver.h"
#include "mouse.h"


// This function redirects function calls from mouse.c to the desired maze solving algorithm
Action solver(Mouse *mouse)
{
    // This can be changed to call other maze solving algorithms
    return leftWallFollower(mouse);
}

// Simple algorithm; mouse goes straight until encountering a wall, then preferentially turns left
Action obstacleAvoider(Mouse *mouse)
{
    if(getFrontReading(mouse) == 0) return FORWARD;
    else if(getLeftReading(mouse) == 0) return LEFT;
    else if(getRightReading(mouse) == 0) return RIGHT;
    else return LEFT;
}

// Left wall following algorithm
int turnedLeft = 0;
Action leftWallFollower(Mouse *mouse)
{
    if(turnedLeft)
    {
        turnedLeft = 0;
        return FORWARD;
    }
    else if(getLeftReading(mouse) == 0)
    {
        turnedLeft = 1;
        return LEFT;
    }
    else if(getFrontReading(mouse) == 0) return FORWARD;
    else return RIGHT;
}

Action floodFill(Mouse *mouse)
{
    // TODO: Implement this function!
    return IDLE;
}