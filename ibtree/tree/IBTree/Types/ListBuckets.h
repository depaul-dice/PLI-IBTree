#ifndef ListBuckets_h
#define ListBuckets_h

#include "BucketTree.h"
#include <set>


struct APtrComp
{
	bool operator()(const BucketTree* lhs, const BucketTree* rhs) const
	{
		if (lhs->bucketID.fNum == rhs->bucketID.fNum)
		{
			return lhs->bucketID.offset < rhs->bucketID.offset;
		}
		else return lhs->bucketID.fNum < rhs->bucketID.fNum;
	}
};

class ListBuckets
{
public:
	std::set<BucketTree*, APtrComp> results;

	~ListBuckets()
	{
		std::set<BucketTree*, APtrComp>::iterator it = results.begin();
		for (; it != results.end(); ++it)
		{
			if ((*it) != 0)
			{
				delete((*it));
			}

		}
	}
};

#endif