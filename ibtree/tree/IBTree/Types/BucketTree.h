#ifndef BucketTree_h
#define BucketTree_h

#include "../../PLITypes/Interval.h"
#include "../../PLITypes/Bucket/Types/BucketInfo.h"

class BucketTree
{
public:
	BucketInfo bucketID;
	Interval interval;
	BucketTree(BucketInfo BucketID, Interval Interval);

	BucketTree(const BucketTree &p2) { bucketID = p2.bucketID; interval = p2.interval; }

	bool operator<(const BucketTree &rhs) const
	{
		if (bucketID.fNum == rhs.bucketID.fNum)
		{
			return bucketID.offset < rhs.bucketID.offset;
		}
		else return bucketID.fNum < rhs.bucketID.fNum;
	}

	bool operator==(const BucketTree &rhs) const
	{
		if (bucketID.fNum == rhs.bucketID.fNum)
		{
			return bucketID.offset == rhs.bucketID.offset;
		}
		else false;
	}
};

#endif