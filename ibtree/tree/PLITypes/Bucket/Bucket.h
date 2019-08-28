#ifndef Bucket_h
#define Bucket_h

#include "Types/BucketInfo.h"
#include "Types/Tuple.h"
#include "../Constants.h"
#include <cstring>

class Bucket
{
public:
	BucketInfo bucketID;
	int Count = 0;
	int length = 0;
	Tuple* tuples[Constants::NUM_ROW_PER_BUCKET];
	Bucket();
	void AddTuple(Tuple* tuple);
};


#endif
