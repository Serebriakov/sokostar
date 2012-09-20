#include "block.h"
#include "constants.h"

/**
 * Inits a new object at the specified location
 *@param _x x coordinate
 *@param _y y coordinate
 */
Block::Block(int _x, int _y): Object(_x, _y) {
    for (int i = 0; i < 4; i++) {
        dir_blocked[i] = true;
    }
}

/**
 * Sets a direction to not be blocked
 *@param direction the direction to set
 *@param the path to reach the spot that can push this block in this direction
 */
void Block::canPush(int direction) {
    dir_blocked[direction] = false;
}

/**
 * Returns whether or not a direction is blocked
 *@param direction the direction to check
 *@return dir_blocked[direction]
 */
bool Block::isBlocked(int direction) {
    return dir_blocked[direction];
}
