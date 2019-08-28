#ifndef IBNode_h
#define IBNode_h

#include "../../PLITypes/Constants.h"
#include "../../PLITypes/Bucket/Types/BucketInfo.h"
#include "../../PLITypes/Interval.h"
#include "../../PLITypes/Data.h"
#include <algorithm>

struct Interval;


class IBNode
{
public:
	int level = 0;
	int length = 0;
	bool isLeaf = true;

	//Entries
	Interval interval[Constants::MAX_NUM_ENTRY];
	BucketInfo bucketID[Constants::MAX_NUM_ENTRY];

	Data max[Constants::MAX_NUM_ENTRY+1];

	//Pointers
	IBNode * pointer[Constants::MAX_NUM_ENTRY+1];

	IBNode();
};

class IBNodeBuffer
{
public:
	int level = 0;
	int length = 0;
	bool isLeaf = true;

	//Entries
	Interval interval[Constants::MAX_NUM_ENTRY+1];
	Data max[Constants::MAX_NUM_ENTRY+1+1];
	BucketInfo bucketID[Constants::MAX_NUM_ENTRY+1+1];

	//Pointers
	IBNode * pointer[Constants::MAX_NUM_ENTRY+1+1];

	IBNodeBuffer();
};


#endif
