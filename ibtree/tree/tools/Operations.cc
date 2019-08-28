#include "Operations.h"



//This function is used to count the bits that are equal to one in an integer.
int countSetBits(unsigned int num)
{
	unsigned int count = 0;
	while (num) {
		count += num & 1;
		num >>= 1;
	}
	return count;
}


//This function is to verify if two intervals intersect and if they do, it returns true. 
bool intersect(Interval& interval1, Interval& interval2)
{
	Data maxV;
	Data minV;
	if (interval1.start >= interval2.start)
	{
		minV = interval1.start;
	}
	else minV = interval2.start;
	if (interval1.end >= interval2.end)
	{
		maxV = interval2.end;
	}
	else
	{
		maxV = interval1.end;
	}
	bool result = (maxV >= minV);
	return result;
}

//This function is to verify whether a float is inside an interval.
bool inside(Interval& interval, Data value)
{
	return ((interval.start <= value) && (interval.end >= value));
}

