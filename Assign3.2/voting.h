#pragma once
#include "vector.h"

int computeSpecificCriticalVotes(Vector<int>& blocks, const int& totalVotes, int specificBlock,
                                 int curIndex, int subSum);
Vector<int> computePowerIndexes(Vector<int>& blocks);
void normalize(Vector<int>& v);
