#ifndef BLOCK_H
#define BLOCK_H

#include "object.h"
#include <vector>

/**
 * Move blocks onto goals to win
 */
class Block: public Object
{
    public:
        /**
         * Inits a new object at the specified location
         *@param _x x coordinate
         *@param _y y coordinate
         */
        Block(int _x, int _y);

        /**
         * Sets a direction to not be blocked
         *@param direction the direction to set
         */
        void canPush(int direction);

        /**
         * Returns whether or not a direction is blocked
         *@param direction the direction to check
         *@return dir_blocked[direction]
         */
        bool isBlocked(int direction);

        /**
         * Returns the path to reach the blcok to push in this direction
         *@return direction the direction you want to push the block
         *@return the path to the block
         */
        //std::vector<int>& getPath(int direction);

        /**
         * Pushes the block in the specified direction
         *@param direction the direction to push
         */
        void push(int direction);

        /**
         * Returns the goal this block is on
         *@return -1 if not on goal, otherwise onGoal
         */
        int getGoal();

        /**
         * Sets the block to be on a goal
         *@param goal the goal to be on
         */
        void setOnGoal(int goal);

    private:
        bool dir_blocked[4]; /**< whether or not each direction is blocked */
        int onGoal; /**< which goal this block is on */
        //std::vector<int> push_paths[4]; /**< the paths to reach each side of the block */
};

#endif // BLOCK_H
