#include "object.h"

/**
 * Inits a new object at the specified location
 *@param _x x coordinate
 *@param _y y coordinate
 */
Object::Object(int _x, int _y) {
    x = _x;
    y = _y;
}

/**
 * Returns the x position of the object
 *@return x
 */
int Object::getX() {
    return x;
}

/**
 * Returns the y position of the object
 *@return y
 */
int Object::getY() {
    return y;
}
