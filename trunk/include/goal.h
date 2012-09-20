#ifndef GOAL_H
#define GOAL_H

#include "object.h"

/**
 * This is where you want to put blocks to win
 */
class Goal: public Object
{
    public:
        /**
         * Inits a new object at the specified location
         *@param _x x coordinate
         *@param _y y coordinate
         */
        Goal(int _x, int _y): Object(_x, _y) {}
};

#endif // GOAL_H
