#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>
#include <map>
#include "block.h"
#include "goal.h"

class Level;

/**
 * A state that the game board can be in
 */
class State
{
    public:
        /**
         * Inits a new state from the level
         */
        State(Level& level);

        /**
         * Frees any memory used by the state
         */
        ~State();

        /**
         * Returns the representation of this state
         *@return value
         */
        std::string get();

        /**
         * Returns the states this state can change into
         *@param children (out) the child states
         */
        void getChildren(std::vector<State *>* children);

        /**
         * Returns the heuristic cost
         *@return h
         */
        int getH();

        /**
         * Returns the actual cost
         *@return g
         */
        int getG();

        /**
         * Returns the estimated cost + actual cost
         */
        int getF();

        /**
         * Returns the parent state
         *@return parent
         */
        State* getParent();

        /**
         * Returns the block that was pushed to get this state
         *@return blockPushed
         */
        int getBlockPushed();

        /**
         * Returns the direction that block was pushed
         *@return pushDirection
         */
        int getPushDirection();

        /**
         * Prints this state
         */
        void print();

    private:
        /**
         * Builds a child state from the parent
         *@param parent the parent state
         *@param block which block was pushed to reach this state
         *@param direction the direction it was pushed
         */
        State(State* _parent, unsigned int block, int direction);

        /**
         * Evaluates (calculates h) how good this state is
         * heuristic = manhatten distance
         */
        void evaluate();

        /**
         * Floodfills the robot's location
         *@param x robot's x coordinate
         *@param y robot's y coordinate
         */
        void floodFill(int x, int y);

        /**
         * Floodfills from a direction
         *@param x robot's x coordinate
         *@param y robot's y coordinate
         *@param dir the direction
         */
        void floodFill(int x, int y, int dir);

        std::vector<Block *> blocks; /**< the blocks in the stage */
        std::map<int, Block *> blocksPos; /**< reference the blocks by position */
        std::vector<Goal *>& goals; /**< the goals to put the blocks on */
        std::string value; /**< the value of the state */
        std::string wall_map; /**< the walls in the stage */

        int width; /**< width of the map */

        int g; /**< the cost to reach this state */
        int h; /**< the heuristic estimate of this state */

        State* parent; /**< the state before this one */
        int blockPushed; /**< which block must be pushed to get this state */
        int pushDirection; /**< which direction that block was pushed */
};

#endif // STATE_H
