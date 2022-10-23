/*
 * File: direction.h
 * ----------------
 * This interface exports an eneumerated type called Direction
 * whose elements includes NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH,
 * SOUTHWEST, WEST and NORTHWEST.
 */

#ifndef DIRECTION_H
#define DIRECTION_H

enum Direction { NORTH, NORTHEAST, EAST, SOUTHEAST,
                 SOUTH, SOUTHWEST, WEST, NORTHWEST };

Direction operator++(Direction & dir); //运算符重载 i++
Direction operator++(Direction & dir, int); //运算符重载 ++i


#endif // DIRECTION_H
