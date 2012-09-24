#include "level.h"
#include "state.h"
#include "constants.h"
#include <cstdio>
#include <string>
#include <cmath>

/**
 * Inits a new level
 */
Level::Level() {
    width = 0;
    height = 0;
    robot = NULL;
    start = NULL;

    states = 0;
}

/**
 * Clears memory used by the level
 */
Level::~Level() {
    if (robot != NULL) {
        delete robot;
        robot = NULL;
    }

    for (unsigned int i = 0; i < goals.size(); i++) {
        delete goals[i];
    }
    goals.clear();
    for (unsigned int i = 0; i < blocks.size(); i++) {
        delete blocks[i];
    }
    blocks.clear();

    if (start != NULL) {
        delete start;
        start = NULL;
    }
}

/**
 * Loads a level from file
 *@param file the file to load from
 *@return true on success, false on failure
 */
bool Level::load(char* file) {
    FILE* f = fopen(file, "r");
    if (f != NULL) {
        int w = 0;
        std::string mapstr = "";
        height = 1;
        while (!feof(f)) {
            char c = fgetc(f);
            mapstr += c;
            if (c == '\n') {
                if (w > width) {
                    width = w;
                }
                height++;
                w = 0;
            }
            w++;
        }
        fclose(f);

        walls = "";
        int x = 0;
        int y = 0;
        for (unsigned int i = 0; i < mapstr.size(); i++) {
            if (mapstr[i] != '\n') {
                // place walls
                walls += mapstr[i] == WALL ? WALL : EMPTY;

                // place additional objects
                switch (mapstr[i]) {
                    case ROBOT:
                        robot = new Robot(x, y);
                        break;
                    case ROBOT_ON_GOAL:
                        robot = new Robot(x, y);
                        goals.push_back(new Goal(x, y));
                        break;
                    case BLOCK:
                        blocks.push_back(new Block(x, y));
                        break;
                    case BLOCK_ON_GOAL:
                        blocks.push_back(new Block(x, y));
                        goals.push_back(new Goal(x, y));
                        break;
                    case GOAL:
                        goals.push_back(new Goal(x, y));
                        break;
                }

                x++;
            } else {
                while (x < width) {
                    walls += EMPTY;
                    x++;
                }
                y++;
                x = 0;
            }
        }

        if (blocks.size() < goals.size()) {
            printf("Error, not enough blocks to cover all goals\n");
            return false;
        }

        start = new State(*this);

        //printf("%s\n", start->get().c_str());

        return true;
    }
    printf("File %s not found\n", file);
    return false;
}

/**
 * Returns the map width
 *@return width
 */
int Level::getWidth() {
    return width;
}

/**
 * Returns the map height
 *@return height
 */
int Level::getHeight() {
    return height;
}

/**
 * Returns the robot
 *@return robot
 */
Robot* Level::getRobot() {
    return robot;
}

/**
 * Returns the goals
 *@return goals
 */
std::vector<Goal *>& Level::getGoals() {
    return goals;
}

/**
 * Returns the blocks
 *@return blocks
 */
std::vector<Block *>& Level::getBlocks() {
    return blocks;
}

/**
 * Returns the starting state
 *@return start
 */
State* Level::getStart() {
    return start;
}

/**
 * Returns the wall map
 *@return wall_map
 */
std::string& Level::getWallMap() {
    return walls;
}

/**
 * Determines the path the robot would need to take to push the block
 *@param block which block to push
 *@param direction the direction to push the block
 *@param x robot x
 *@param y robot y
 *@param blockSet which set of blocks to use
 */
/*void Level::pathTo(int block, int direction, int x, int y, std::vector<Block *>* blockSet) {
    int bx = (*blockSet)[block]->getX();
    int by = (*blockSet)[block]->getY();
    switch (direction) {
        case UP:
            by++;
            break;
        case DOWN:
            by--;
            break;
        case LEFT:
            bx++;
            break;
        case RIGHT:
            bx--;
            break;
    }

    //printf("pathTo(%dx%d => %dx%d)\n", x, y, bx, by);

    int goal = bx+by*width;
    int start = x+y*width;

    std::map<int, int> closedset;
    std::map<int, int> openset;
    std::map<int, int> g_costs;
    openset[start] = evaluate(start, bx, by);
    g_costs[start] = 0;
    std::map<int, int> came_from;

    //A* states
    while (!openset.empty()) {
        int current = best(openset);

        closedset[current] = openset[current];
        openset.erase(current);

        if (current == goal) { // goal
            // reconstruct path
            while (came_from.count(current)) {
                int dir = UP;
                int difference = came_from[current]-current;
                if (difference == -1) {
                    dir = RIGHT;
                } else if (difference == 1) {
                    dir = LEFT;
                } else if (difference == -width) {
                    dir = DOWN;
                }
                //path->push_back(dir);
                //printf("%d ", dir);
                (*blockSet)[block]->getPath(direction).push_back(dir);
                current = came_from[current];
            }
            break;
        }

        for (int i = 0; i < 4; i++) {
            int child = current;
            switch (i) {
                case UP:
                    child -= width;
                    break;
                case DOWN:
                    child += width;
                    break;
                case LEFT:
                    child--;
                    break;
                case RIGHT:
                    child++;
                    break;
            }

            bool invalidSquare = false;
            for (unsigned int j = 0; j < blockSet->size(); j++) {
                if (child == (*blockSet)[j]->getX()+(*blockSet)[j]->getY()*width) {
                    invalidSquare = true;
                    break;
                }
            }

            if (invalidSquare || walls[child] != EMPTY || closedset.count(child)) {
                continue;
            }
            int tentative_g = g_costs[current]+1;

            if (!openset.count(child) || tentative_g < g_costs[child]) {
                came_from[child] = current;
                g_costs[child] = tentative_g;
                openset[child] = g_costs[child]+evaluate(child, bx, by);
            }
        }
    }

    //printf("\n");
}*/

/**
 * Pushes a block, updating the level state itself
 *@param block which block to push
 *@param direction the direction to push the block
 *@param path (out) the path the robot must take to make the push (in reverse)
 */
void Level::pushBlock(int block, int direction, std::vector<int>* path) {
    int bx = blocks[block]->getX();
    int by = blocks[block]->getY();
    switch (direction) {
        case UP:
            by++;
            break;
        case DOWN:
            by--;
            break;
        case LEFT:
            bx++;
            break;
        case RIGHT:
            bx--;
            break;
    }

    //printf("pathTo(%dx%d => %dx%d)\n", x, y, bx, by);

    int goal = bx+by*width;
    int start = robot->getX()+robot->getY()*width;

    std::map<int, int> closedset;
    std::map<int, int> openset;
    std::map<int, int> g_costs;
    openset[start] = evaluate(start, bx, by);
    g_costs[start] = 0;
    std::map<int, int> came_from;

    //A* states
    while (!openset.empty()) {
        int current = best(openset);

        closedset[current] = openset[current];
        openset.erase(current);

        states++;

        if (current == goal) { // goal
            // reconstruct path
            while (came_from.count(current)) {
                int dir = UP;
                int difference = came_from[current]-current;
                if (difference == -1) {
                    dir = RIGHT;
                } else if (difference == 1) {
                    dir = LEFT;
                } else if (difference == -width) {
                    dir = DOWN;
                }
                path->push_back(dir);
                //printf("%d ", dir);
                //blocks[block]->getPath(direction).push_back(dir);
                current = came_from[current];
            }
            break;
        }

        for (int i = 0; i < 4; i++) {
            int child = current;
            switch (i) {
                case UP:
                    child -= width;
                    break;
                case DOWN:
                    child += width;
                    break;
                case LEFT:
                    child--;
                    break;
                case RIGHT:
                    child++;
                    break;
            }

            bool invalidSquare = false;
            for (unsigned int j = 0; j < blocks.size(); j++) {
                if (child == blocks[j]->getX()+blocks[j]->getY()*width) {
                    invalidSquare = true;
                    break;
                }
            }

            if (invalidSquare || walls[child] != EMPTY || closedset.count(child)) {
                continue;
            }
            int tentative_g = g_costs[current]+1;

            if (!openset.count(child) || tentative_g < g_costs[child]) {
                came_from[child] = current;
                g_costs[child] = tentative_g;
                openset[child] = g_costs[child]+evaluate(child, bx, by);
            }
        }
    }

    robot->placeAt(blocks[block]->getX(), blocks[block]->getY());
    blocks[block]->push(direction);
}

/**
 * Returns how many robot states were opened
 *@return states
 */
int Level::getRobotStatesExpanded() {
    return states;
}

/**
 * Grabs the best guess from the openset
 *@param openset the open set
 *@return the bets guess of where to go next
 */
int Level::best(std::map<int, int>& openset) {
    int best = -1;
    int bestF = -1;
    for (std::map<int, int>::iterator i = openset.begin(); i != openset.end(); i++) {
        if (best == -1 || i->second < bestF) {
            bestF = i->second;
            best = i->first;
        }
    }
    return best;
}

/**
 * Evaluates (calculates h) how good this state is
 * heuristic = manhatten distance
 *@param index the location of the robot
 *@param bx destination x
 *@param by destination y
 */
int Level::evaluate(int index, int bx, int by) {
    return std::abs(index/width-by)+std::abs((index%width)-bx);
}
