#include "state.h"
#include "level.h"
#include <cmath>
#include "constants.h"

#include <cstdio>

/**
 * Inits a new state from the level
 */
State::State(Level& level): goals(level.getGoals()) {
    parent = NULL;
    blockPushed = -1;
    pushDirection = -1;

    width = level.getWidth();

    // build state representation
    wall_map = level.getWallMap();
    value = *wall_map;
    value[level.getRobot()->getX()+level.getRobot()->getY()*width] = ROBOT;
    for (unsigned int i = 0; i < level.getBlocks().size(); i++) {
        int index = level.getBlocks()[i]->getX()+level.getBlocks()[i]->getY()*width;
        value[index] = BLOCK;
        blocks.push_back(new Block(level.getBlocks()[i]->getX(), level.getBlocks()[i]->getY()));
        blocksPos[index] = blocks[i];
    }

    for (unsigned int i = 0; i < goals.size(); i++) {
        int index = goals[i]->getX()+goals[i]->getY()*width;
        if (value[index] == ROBOT) {
            value[index] = ROBOT_ON_GOAL;
        } else if (value[index] == BLOCK) {
            value[index] = BLOCK_ON_GOAL;
        } else {
            value[index] = GOAL;
        }
    }

    floodFill(level.getRobot()->getX(), level.getRobot()->getY());

    /*for (int i = 0; i < blocks.size(); i++) {
        printf("block %d can be pushed up?: %d\n", i, !blocks[i]->isBlocked(UP));
        printf("block %d can be pushed down?: %d\n", i, !blocks[i]->isBlocked(DOWN));
        printf("block %d can be pushed left?: %d\n", i, !blocks[i]->isBlocked(LEFT));
        printf("block %d can be pushed right?: %d\n", i, !blocks[i]->isBlocked(RIGHT));
    }*/

    g = 0;
    evaluate();
}

/**
 * Frees any memory used by the state
 */
State::~State() {
    for (unsigned int i = 0; i < blocks.size(); i++) {
        delete blocks[i];
    }
    blocksPos.clear();
    blocks.clear();
}

/**
 * Returns the representation of this state
 *@return value
 */
std::string State::get() {
    return value;
}

/**
 * Floodfills the robot's location
 *@param x robot's x coordinate
 *@param y robot's y coordinate
 */
void State::floodFill(int x, int y) {
    floodFill(x-1, y, LEFT);
    floodFill(x+1, y, RIGHT);
    floodFill(x, y-1, UP);
    floodFill(x, y+1, DOWN);
}

/**
 * Floodfills from a direction
 *@param x robot's x coordinate
 *@param y robot's y coordinate
 *@param dir the direction
 */
void State::floodFill(int x, int y, int dir) {
    int index = x+y*width;
    if (value[index] == EMPTY || value[index] == GOAL) {
        if (value[index] == EMPTY) {
            value[index] = ROBOT;
        } else {
            value[index] = ROBOT_ON_GOAL;
        }

        if (dir != RIGHT) {
            floodFill(x-1, y, LEFT);
        }
        if (dir != LEFT) {
            floodFill(x+1, y, RIGHT);
        }
        if (dir != DOWN) {
            floodFill(x, y-1, UP);
        }
        if (dir != UP) {
            floodFill(x, y+1, DOWN);
        }
    } else if (value[index] == BLOCK || value[index] == BLOCK_ON_GOAL) {
        // check if the block can be pushed from here
        switch (dir) {
            case UP:
                if (value[index-width] != WALL && value[index-width] != BLOCK && value[index-width] != BLOCK_ON_GOAL) {
                    blocksPos[index]->canPush(UP);
                }
                break;
            case DOWN:
                if (value[index+width] != WALL && value[index+width] != BLOCK && value[index+width] != BLOCK_ON_GOAL) {
                    blocksPos[index]->canPush(DOWN);
                }
                break;
            case LEFT:
                if (value[index-1] != WALL && value[index-1] != BLOCK && value[index-1] != BLOCK_ON_GOAL) {
                    blocksPos[index]->canPush(LEFT);
                }
                break;
            case RIGHT:
                if (value[index+1] != WALL && value[index+1] != BLOCK && value[index+1] != BLOCK_ON_GOAL) {
                    blocksPos[index]->canPush(RIGHT);
                }
                break;
        }
    }
}

/**
 * Returns the states this state can change into
 *@param children (out) the child states
 */
void State::getChildren(std::vector<State *>* children) {
    for (unsigned int i = 0; i < blocks.size(); i++) {
        for (int j = 0; j < 4; j++) {
            if (!blocks[i]->isBlocked(j)) {
                children->push_back(new State(this, i, j));
            }
        }
    }
}

/**
 * Returns the heuristic cost
 *@return h
 */
int State::getH() {
    return h;
}

/**
 * Returns the actual cost
 *@return g
 */
int State::getG() {
    return g;
}

/**
 * Returns the estimated cost + actual cost
 */
int State::getF() {
    return g+h;
}

/**
 * Builds a child state from the parent
 *@param parent the parent state
 *@param block which block was pushed to reach this state
 *@param direction the direction it was pushed
 */
State::State(State* _parent, unsigned int block, int direction): goals(_parent->goals) {
    parent = _parent;
    blockPushed = block;
    pushDirection = direction;

    width = parent->width;

    // build state representation
    wall_map = parent->wall_map;
    value = *wall_map;

    // place the robot where the block was
    value[parent->blocks[block]->getX()+parent->blocks[block]->getY()*width] = ROBOT;

    // place the blocks, make sure to move one
    for (unsigned int i = 0; i < parent->blocks.size(); i++) {
        int index = parent->blocks[i]->getX()+parent->blocks[i]->getY()*width;
        if (i == block) {
            switch (direction) {
                case UP:
                    index -= width;
                    break;
                case DOWN:
                    index += width;
                    break;
                case LEFT:
                    index--;
                    break;
                case RIGHT:
                    index++;
                    break;
            }
        }
        value[index] = BLOCK;
        blocks.push_back(new Block(index%width, index/width));
        blocksPos[index] = blocks[i];
    }

    for (unsigned int i = 0; i < goals.size(); i++) {
        int index = goals[i]->getX()+goals[i]->getY()*width;
        if (value[index] == ROBOT) {
            value[index] = ROBOT_ON_GOAL;
        } else if (value[index] == BLOCK) {
            value[index] = BLOCK_ON_GOAL;
        } else {
            value[index] = GOAL;
        }
    }

    floodFill(parent->blocks[block]->getX(), parent->blocks[block]->getY());

    /*for (int i = 0; i < blocks.size(); i++) {
        printf("block %d can be pushed up?: %d\n", i, !blocks[i]->isBlocked(UP));
        printf("block %d can be pushed down?: %d\n", i, !blocks[i]->isBlocked(DOWN));
        printf("block %d can be pushed left?: %d\n", i, !blocks[i]->isBlocked(LEFT));
        printf("block %d can be pushed right?: %d\n", i, !blocks[i]->isBlocked(RIGHT));
    }*/

    g = parent->g+1;
    evaluate();
}

/**
 * Evaluates (calculates h) how good this state is
 * heuristic = manhatten distance
 */
void State::evaluate() {
    h = 0;
    // blocks can be re-counted, we just care how close a block is to this goal
    for (unsigned int i = 0; i < goals.size(); i++) {
        int distance = -1;
        for (unsigned int j = 0; j < blocks.size(); j++) {
            int new_distance = std::abs(blocks[j]->getX()-goals[i]->getX())+std::abs(blocks[j]->getY()-goals[i]->getY());
            if (distance == -1 || new_distance < distance) {
                distance = new_distance;
                if (distance == 0) { // can't get better than this
                    break;
                }
            }
        }
        h += distance;
    }
}

/**
 * Returns the parent state
 *@return parent
 */
State* State::getParent() {
    return parent;
}

/**
 * Returns the block that was pushed to get this state
 *@return blockPushed
 */
int State::getBlockPushed() {
    return blockPushed;
}

/**
 * Returns the direction that block was pushed
 *@return pushDirection
 */
int State::getPushDirection() {
    return pushDirection;
}

/**
 * Prints this state
 */
void State::print() {
    for (unsigned int i = 0; i < value.size(); i++) {
        printf("%c", value[i]);
        if (!((i+1)%width)) {
            printf("\n");
        }
    }
    printf("f(%d) = g(%d) + h(%d)\n", g+h, g, h);
}
