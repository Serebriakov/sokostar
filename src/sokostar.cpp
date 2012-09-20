#include "sokostar.h"
#include <vector>
#include <cstdio>
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
    return level.load(file);
}

/**
 * Solves the problem
 */
void SokoStar::solve() {
    std::map<std::string, State *> closedset;
    std::map<std::string, State *> openset;
    openset[level.getStart()->get()] = level.getStart();
    std::map<std::string, std::string> came_from;

    while (!openset.empty()) {
        State* current = best(openset);

        openset.erase(current->get());
        closedset[current->get()] = current;

        if (current->getH() == 0) { // goal
            buildPath(current);
            break;
        }

        std::vector<State *> children;
        current->getChildren(&children);
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
    if (!rBlocksPushed.size()) {
        printf("This level has no solution\n");
    } else {
        for (int i = (int)rBlocksPushed.size()-1; i >= 0; i--) {
            printf("Push block(%d) %s\n", rBlocksPushed[i], asDirection(rPushDirection[i]));
        }
    }
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
            rBlocksPushed.push_back(node->getBlockPushed());
            rPushDirection.push_back(node->getPushDirection());
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
