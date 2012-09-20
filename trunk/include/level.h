#ifndef LEVEL_H
#define LEVEL_H

#include "robot.h"
#include "goal.h"
#include "block.h"
#include <vector>

class State;

/**
 * A Sokoban level
 */
class Level
{
    public:
        /**
         * Inits a new level
         */
        Level();

        /**
         * Clears memory used by the level
         */
        ~Level();

        /**
         * Loads a level from file
         *@param file the file to load from
         *@return true on success, false on failure
         */
        bool load(char* file);

        /**
         * Returns the map
         *@return map
         */
        int* getMap();

        /**
         * Returns the map width
         *@return width
         */
        int getWidth();

        /**
         * Returns the map height
         *@return height
         */
        int getHeight();

        /**
         * Returns the robot
         *@return robot
         */
        Robot* getRobot();

        /**
         * Returns the goals
         *@return goals
         */
        std::vector<Goal *>& getGoals();

        /**
         * Returns the blocks
         *@return blocks
         */
        std::vector<Block *>& getBlocks();

        /**
         * Returns the starting state
         *@return start
         */
        State* getStart();

    private:
        int width; /**< width of the level */
        int height; /**< height of the level */
        int* map; /**< the level itself */

        Robot* robot; /**< there is exactly 1 robot in a level */
        std::vector<Goal *> goals; /**< there can be many goals */
        std::vector<Block *> blocks; /**< there can be many blocks */

        State* start; /**< the initial state */
};

#endif // LEVEL_H
