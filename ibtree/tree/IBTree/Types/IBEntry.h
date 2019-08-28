#ifndef IBEntry_h
#define IBEntry_h

#include "../../PLITypes/Constants.h"
#include "../../PLITypes/Bucket/Types/BucketInfo.h"
#include "../../PLITypes/Interval.h"
#include "../../PLITypes/Data.h"


class IBEntry
{
public:
	Interval  interval;
	Data max = 0.0;
	BucketInfo bucketID;

	IBEntry();

	IBEntry(Interval interval, float Max, BucketInfo BucketID);
};



#endif
