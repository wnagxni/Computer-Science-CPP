/*
 * File: direction.cpp
 * -------------------
 * The file implement the interfaces in direction.h.
 */

#include "direction.h"

Direction operator++(Direction & dir) { //前缀形式
    dir = Direction ((dir + 1) % 8); //顺时针循环
    return dir;
}

Direction operator++(Direction & dir, int) { //后缀形式
    Direction oldDir = dir; //保存
    dir = Direction((dir + 1) % 8);
    return oldDir;
}
