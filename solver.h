#ifndef SOLVER_H
#define SOLVER_H

#include "mouse.h"

Action solver(Mouse *mouse);
Action leftWallFollower(Mouse *mouse);
Action floodFill(Mouse *mouse);

#endif