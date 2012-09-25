#include "sokostar.h"
#include <vector>
#include <cstdio>
#include <sys/time.h>
#include "constants.h"

/**
 * Inits a new SokoStar solver
 */
SokoStar::SokoStar() {
}

/**
 * Loads a level from the file
 *@param file the file to load th level from
 *@return true on success, false if there was a problem loading the level
 */
bool SokoStar::load(char* file) {
    printf("Loading file: %s\n", file);
    return level.load(file);
}

/**
 * Solves the problem
 */
void SokoStar::solve() {
    std::map<std::string, State *> closedset;
    std::map<std::string, State *> openset;
    openset[level.getStart()->get()] = level.getStart();

    printf("Searching for a solution...\n");

    msec = 0;
    states = 0;
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);

    //A* states
    while (!openset.empty()) {
        State* current = best(openset);

        openset.erase(current->get());
        closedset[current->get()] = current;

        states++;

        if (current->getH() == 0) { // goal
            buildPath(current);
            break;
        }

        std::vector<State *> children;
        current->getChildren(&children, level);
        for (unsigned int i = 0; i < children.size(); i++) {
            if (closedset.count(children[i]->get())) {
                continue;
            }

            if (!openset.count(children[i]->get()) || children[i]->getG() < openset[children[i]->get()]->getG()) {
                if (openset.count(children[i]->get())) {
                    delete openset[children[i]->get()];
                }
                openset[children[i]->get()] = children[i];
            }
        }
    }

    // now get the robot paths
    for (int i = (int)rBlocksPushed.size()-1; i >= 0; i--) {
        rRobotMovements.push_back(std::vector<int>());
        level.pushBlock(rBlocksPushed[i], rPushDirection[i], &rRobotMovements[rRobotMovements.size()-1]);
    }

    gettimeofday(&end, NULL);
    msec = (end.tv_sec-start.tv_sec)*1000.f+(end.tv_usec-start.tv_usec)/1000.f;
    //msec = clock()-start;///CLOCKS_PER_SEC*1000;

    for (std::map<std::string, State *>::iterator i = openset.begin(); i != openset.end(); i++) {
        delete i->second;
    }
    for (std::map<std::string, State *>::iterator i = closedset.begin(); i != closedset.end(); i++) {
        if (i->first != level.getStart()->get()) {
            delete i->second;
        }
    }


}

/**
 * Prints the solution to stdout
 */
void SokoStar::printSolution() {
    FILE* f = fopen("results.txt", "w");
    if (f == NULL) {
        fprintf(f, "Error writing results to results.txt\n");
    }
    printf("\n");
    if (!rBlocksPushed.size()) {
        printf("This level has no solution\n");
        if (f != NULL) {
            fprintf(f, "This level has no solution\n");
        }
    } else {
        int step = 1;
        //printf("\nSolution:\n");
        for (int i = (int)rBlocksPushed.size()-1; i >= 0; i--) {
            for (int j = (int)rRobotMovements[rRobotMovements.size()-i-1].size()-1; j >= 0; j--) {
                printf("%d. Move %s\n", step, asDirection(rRobotMovements[rRobotMovements.size()-i-1][j]));
                //printf("%c", asDirection(rRobotMovements[rRobotMovements.size()-i-1][j])[0]-'A'+'a');
                if (f != NULL) {
                    fprintf(f, "%d. Move %s\n", step, asDirection(rRobotMovements[rRobotMovements.size()-i-1][j]));
                }
                step++;
            }
            printf("%d. Push block(%d) %s\n", step, rBlocksPushed[i], asDirection(rPushDirection[i]));
            //printf("%c", asDirection(rPushDirection[i])[0]);
            if (f != NULL) {
                fprintf(f, "%d. Push block(%d) %s\n", step, rBlocksPushed[i], asDirection(rPushDirection[i]));
            }
            step++;
        }
        printf("\n");
        if (f != NULL) {
            fprintf(f, "\n");
        }
        //printf("\n\nnote: solution is in a common solution format\n(see: http://sokobano.de/wiki/index.php?title=Level_format#Solution)\n\n");
    }
    if (msec <= 0.f) {
        printf("Solver was too fast (~0 msec), could not measure time\n");
        if (f != NULL) {
            fprintf(f, "Solver was too fast (~0 msec), could not measure time\n");
        }
    } else {
        printf("Solver took %f msec\n", msec);
        if (f != NULL) {
            fprintf(f, "Solver took %f msec\n", msec);
        }
    }
    printf("%d Block States were expanded\n", states);
    printf("%d Robot states were expanded\n", level.getRobotStatesExpanded());
    if (f != NULL) {
        fprintf(f, "%d Block States were expanded\n", states);
        fprintf(f, "%d Robot states were expanded\n", level.getRobotStatesExpanded());

        fclose(f);
    }

    printf("\nResults also written to results.txt\n");
}

/**
 * Grabs the best guess from the openset
 *@param openset the open set
 *@return the bets guess of where to go next
 */
State* SokoStar::best(std::map<std::string, State *>& openset) {
    State* best = NULL;
    for (std::map<std::string, State *>::iterator i = openset.begin(); i != openset.end(); i++) {
        if (best == NULL || i->second->getF() < best->getF()) {
            best = i->second;
        }
    }
    return best;
}

/**
 * Builds the path from the end state to the start state
 *@param node the end state
 */
void SokoStar::buildPath(State* node) {
    while (node != NULL) {
        //node->print();
        //printf("\n");
        if (node->getParent() != NULL) { //ie- not the start node
            rBlocksPushed.push_back(node->getBlockPushedIndex());
            rPushDirection.push_back(node->getPushDirection());
            //rRobotMovements.push_back(node->getBlockPushedPath());
        }
        node = node->getParent();
    }
}

/**
 * Translates the direction constant to a string
 *@param direction the direction to translate
 *@return a string
 */
const char* SokoStar::asDirection(int direction) {
    switch (direction) {
        case UP: return "UP";
        case DOWN: return "DOWN";
        case LEFT: return "LEFT";
        case RIGHT: return "RIGHT";
    }
    return "????";
}
