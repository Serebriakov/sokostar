#ifndef OBJECT_H
#define OBJECT_H

/**
 * Objects in Sokoban have an x and y coordinate
 */
class Object
{
    public:
        /**
         * Inits a new object at the specified location
         *@param _x x coordinate
         *@param _y y coordinate
         */
        Object(int _x, int _y);

        /**
         * Returns the x position of the object
         *@return x
         */
        int getX();

        /**
         * Returns the y position of the object
         *@return y
         */
        int getY();

    protected:
        int x; /**< x coordinate */
        int y; /**< y coordinate */
};

#endif // OBJECT_H
