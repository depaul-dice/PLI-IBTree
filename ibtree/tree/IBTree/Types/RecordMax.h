#ifndef RecordMax_h
#define RecordMax_h

#include "../../PLITypes/Data.h"

class recordMax
{
public:
	Data rightMax = 0.0;
	Data leftMax = 0.0;
	recordMax();
	recordMax(float CurMax, float NewMax);
};

#endif