#ifndef ROBOT_H
#define ROBOT_H

#include "object.h"

/**
 * Sokoban robot pushes blocks
 */
class Robot: public Object
{
    public:
        /**
         * Inits a new object at the specified location
         *@param _x x coordinate
         *@param _y y coordinate
         */
        Robot(int _x, int _y): Object(_x, _y) {}

        /**
         * Places the robot at a new location
         *@param _x x coordinate
         *@param _y y coordinate
         */
        void placeAt(int _x, int _y);
};

#endif // ROBOT_H
