#ifndef SOKOSTAR_H
#define SOKOSTAR_H

#include "level.h"
#include "state.h"
#include <map>
#include <string>

/**
 * Solves Sokoban problems
 */
class SokoStar
{
    public:
        /**
         * Inits a new SokoStar solver
         */
        SokoStar();

        /**
         * Loads a level from the file
         *@param file the file to load th level from
         *@return true on success, false if there was a problem loading the level
         */
        bool load(char* file);

        /**
         * Solves the problem
         */
        void solve();

        /**
         * Prints the solution to stdout
         */
        void printSolution();

    private:
        /**
         * Grabs the best guess from the openset
         *@param openset the open set
         *@return the bets guess of where to go next
         */
        State* best(std::map<std::string, State *>& openset);

        /**
         * Builds the path from the end state to the start state
         *@param node the end state
         */
        void buildPath(State* node);

        /**
         * Translates the direction constant to a string
         *@param direction the direction to translate
         *@return a string
         */
        const char* asDirection(int direction);

        Level level; /**< the sokoban level to solve */

        std::vector<int> rBlocksPushed; /**< the blocks we push (in reverse) */
        std::vector<int> rPushDirection; /**< the direction we push the blocks (in reverse) */
};

#endif // SOKOSTAR_H
