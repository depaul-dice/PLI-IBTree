#ifndef Interval_h
#define Interval_h

#include "Data.h"

struct Interval {
public:
	Data start;
	Data end;
	Interval();
	Interval(Data Start, Data End);
	Interval(const Interval &p2) { start = p2.start; end = p2.end; }
};

#endif