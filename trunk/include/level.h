#ifndef LEVEL_H
#define LEVEL_H

#include "robot.h"
#include "goal.h"
#include "block.h"
#include <vector>
#include <string>
#include <map>

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

        /**
         * Returns the wall map
         *@return wall_map
         */
        std::string& getWallMap();

        /**
         * Determines the path the robot would need to take to push the block
         *@param block which block to push
         *@param direction the direction to push the block
         *@param x robot x
         *@param y robot y
         *@param blockSet which set of blocks to use
         */
        //void pathTo(int block, int direction, int x, int y, std::vector<Block *>* blockSet);

        /**
         * Pushes a block, updating the level state itself
         *@param block which block to push
         *@param direction the direction to push the block
         *@param path (out) the path the robot must take to make the push
         */
        void pushBlock(int block, int direction, std::vector<int>* path);

        /**
         * Returns how many robot states were opened
         *@return states
         */
        int getRobotStatesExpanded();

    private:
        /**
         * Grabs the best guess from the openset
         *@param openset the open set
         *@return the bets guess of where to go next
         */
        int best(std::map<int, int>& openset);

        /**
         * Evaluates (calculates h) how good this state is
         * heuristic = manhatten distance
         *@param index the location of the robot
         *@param bx destination x
         *@param by destination y
         */
        int evaluate(int index, int bx, int by);

        int width; /**< width of the level */
        int height; /**< height of the level */
        std::string walls; /**< the walls */

        Robot* robot; /**< there is exactly 1 robot in a level */
        std::vector<Goal *> goals; /**< there can be many goals */
        std::vector<Block *> blocks; /**< there can be many blocks */

        State* start; /**< the initial state */

        // diagnostic
        int states; /**< how many robot states are considered */
};

#endif // LEVEL_H
