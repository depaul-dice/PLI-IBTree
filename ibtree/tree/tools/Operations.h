#ifndef Operations_h
#define Operations_h


#include "../PLITypes/Interval.h"
#include "../PLITypes/Data.h"



int countSetBits(unsigned int num);


bool intersect(Interval& interval1, Interval& interval2);


bool inside(Interval& interval, Data value);

#endif
