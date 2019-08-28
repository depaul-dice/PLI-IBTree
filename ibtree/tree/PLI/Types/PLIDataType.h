#ifndef PLIDataType_h
#define PLIDataType_h

#include "../../PLITypes/Constants.h"
#include "../../PLITypes/Interval.h"
#include <stdlib.h>



class clusteredData
{
public:
	Interval* interval;
	float* bucketID;
	clusteredData();
	~clusteredData();
};


class overflowData
{
public:
	long pointer;
	float max;
	float min;
	overflowData();
};


#endif
