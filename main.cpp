#include <cstdio>
#include "sokostar.h"

/**
 * main entry point for SokoStar
 *@param argc number of args passed to program
 *@param argv passed args, should pass the filename for the sokoban map to load
 *@return 0 on success
 */
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Error: format is\n\t%s SOKOBAN_PROBLEM\n", argv[0]);
        printf("Level file format is the most common representation, described at\nhttp://www.sokoban-online.de/help/sokoban/level-format.html");
        return -1;
    }

    SokoStar solver;
    if (solver.load(argv[1])) {
        solver.solve();
        solver.printSolution();
    } else {
        printf("There was an error loading the level. Make sure the file exists and is in the correct format\nhttp://www.sokoban-online.de/help/sokoban/level-format.html");
        return -2;
    }

    return 0;
}
