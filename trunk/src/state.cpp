#include "state.h"
#include "level.h"
#include <cmath>
#include "constants.h"

#include <cstdio>

/**
 * Inits a new state from the level
 */
State::State(Level& level)/*: goals(level.getGoals())*/ {
    parent = NULL;
    blockPushed = -1;
    pushDirection = -1;

    width = level.getWidth();

    // build state representation
    //wall_map = level.getWallMap();
    value = level.getWallMap();//*wall_map;
    value[level.getRobot()->getX()+level.getRobot()->getY()*width] = ROBOT;
    for (unsigned int i = 0; i < level.getBlocks().size(); i++) {
        int index = level.getBlocks()[i]->getX()+level.getBlocks()[i]->getY()*width;
        value[index] = BLOCK;
        blocks.push_back(new Block(level.getBlocks()[i]->getX(), level.getBlocks()[i]->getY()));
        blocksPos[index] = blocks[i];
    }

    for (unsigned int i = 0; i < level.getGoals().size(); i++) {
        int index = level.getGoals()[i]->getX()+level.getGoals()[i]->getY()*width;
        if (value[index] == ROBOT) {
            value[index] = ROBOT_ON_GOAL;
        } else if (value[index] == BLOCK) {
            value[index] = BLOCK_ON_GOAL;
        } else {
            value[index] = GOAL;
        }
    }

    g = 0;
    evaluate(level.getGoals());

    if (h) {
        floodFill(level.getRobot()->getX(), level.getRobot()->getY());
    }
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
 *@param level the level these states are in
 */
void State::getChildren(std::vector<State *>* children, Level& level) {
    for (unsigned int i = 0; i < blocks.size(); i++) {
        for (int j = 0; j < 4; j++) {
            if (!blocks[i]->isBlocked(j)) {
                children->push_back(new State(this, level, i, j));
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
 *@param level the level that this state is in
 *@param block which block was pushed to reach this state
 *@param direction the direction it was pushed
 */
State::State(State* _parent, Level& level, unsigned int block, int direction)/*: goals(_parent->goals)*/ {
    parent = _parent;
    blockPushed = block;
    pushDirection = direction;

    width = parent->width;

    // build state representation
    //wall_map = parent->wall_map;
    value = level.getWallMap();//*wall_map;

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

    for (unsigned int i = 0; i < level.getGoals().size(); i++) {
        int index =  level.getGoals()[i]->getX()+ level.getGoals()[i]->getY()*width;
        if (value[index] == ROBOT) {
            value[index] = ROBOT_ON_GOAL;
        } else if (value[index] == BLOCK) {
            value[index] = BLOCK_ON_GOAL;
        } else {
            value[index] = GOAL;
        }
    }

    //printf("parent[%d,%d] => me[%d,%d]\n", parent->blockPushed, parent->pushDirection, block, direction);

    //g = parent->g+1;
    g = parent->g;
    if (parent->blockPushed != blockPushed) {
        g += level.getGoals().size();
    }
    /*if (parent->parent == NULL) { // parent is start state
        level.pathTo(block, direction, level.getRobot()->getX(), level.getRobot()->getY(), &(parent->blocks));
    } else {
        level.pathTo(block, direction, parent->parent->blocks[parent->blockPushed]->getX(), parent->parent->blocks[parent->blockPushed]->getY(), &(parent->blocks));
    }
    g = parent->g+parent->blocks[block]->getPath(direction).size();*/
    evaluate(level.getGoals());

    if (h) {
        floodFill(parent->blocks[block]->getX(), parent->blocks[block]->getY());
    }
}

/**
 * Evaluates (calculates h) how good this state is
 * heuristic = manhatten distance
 *@param goals the goals we want to cover
 */
void State::evaluate(std::vector<Goal *>& goals) {
    h = 0;
    // blocks can be re-counted, we just care how close a block is to this goal
    for (unsigned int i = 0; i < goals.size(); i++) {
        for (unsigned int j = 0; j < blocks.size(); j++) {
            if (std::abs(blocks[j]->getX()-goals[i]->getX())+std::abs(blocks[j]->getY()-goals[i]->getY()) == 0) {
                blocks[j]->setOnGoal(i);
            }
        }
    }

    for (unsigned int i = 0; i < goals.size(); i++) {
        int distance = -1;
        int blockJ = -1;
        for (unsigned int j = 0; j < blocks.size(); j++) {
            if (blocks[j]->getGoal() == (signed)i) {
                distance = 0;
                break;
            } else if (blocks[j]->getGoal() == -1) {
                int new_distance = std::abs(blocks[j]->getX()-goals[i]->getX())+std::abs(blocks[j]->getY()-goals[i]->getY());
                if (distance == -1 || new_distance < distance) {
                    distance = new_distance;
                    blockJ = i;
                    if (distance == 0) { // can't get better than this
                        break;
                    }
                }
            }
        }
        if (distance != 0) {
            h += distance+1;
        }
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
int State::getBlockPushedIndex() {
    return blockPushed;
}

/**
 * Returns the path to reach the side of the block to push it
 *@return the path to the block
 */
/*std::vector<int>& State::getBlockPushedPath() {
    return parent->blocks[blockPushed]->getPath(pushDirection);
}*/

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
