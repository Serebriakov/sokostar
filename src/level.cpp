#include "level.h"
#include "state.h"
#include "constants.h"
#include <cstdio>
#include <string>

/**
 * Inits a new level
 */
Level::Level() {
    width = 0;
    height = 0;
    map = NULL;
    robot = NULL;
    start = NULL;
}

/**
 * Clears memory used by the level
 */
Level::~Level() {
    if (map != NULL) {
        delete map;
        map = NULL;
    }
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

        map = new int[width*height];
        int x = 0;
        int y = 0;
        for (unsigned int i = 0; i < mapstr.size(); i++) {
            if (mapstr[i] != '\n') {
                // place walls
                map[y*width+x] = mapstr[i] == WALL ? WALL : EMPTY;

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
                    map[y*width+x] = EMPTY;
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

        /*for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                printf("%c", map[i*width+j]);
            }
            printf("\n");
        }*/

        start = new State(*this);

        //printf("%s\n", start->get().c_str());

        return true;
    }
    printf("File %s not found\n", file);
    return false;
}

/**
 * Returns the map
 *@return map
 */
int* Level::getMap() {
    return map;
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
