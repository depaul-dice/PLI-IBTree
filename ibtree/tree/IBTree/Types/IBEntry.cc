#include "IBEntry.h"

//This function initializes an IBEntry object.
IBEntry::IBEntry()
	{
		interval.start=0.0;
		interval.end=0.0;
		max = 0.0;
		bucketID.offset = 0;
	}

//This function initializes an IBEntry object with some given parameters.
IBEntry::IBEntry(Interval intervalt, float Max, BucketInfo BucketID)
	{
	interval.start=intervalt.start;
	interval.end=intervalt.end;
		max = Max;
		bucketID = BucketID;
	}
