#ifndef ThreadInfo_h
#define ThreadInfo_h

#include "../../PLITypes/Interval.h"
#include "../../IBTree/Types/ListBuckets.h"
#include "ReturningList.h"
#include <set>
#include <mutex>
#include <condition_variable>


class ThreadInfo
{
public:
	ReturningList * OutputTuples = 0;
	Interval query;
	ListBuckets* b = 0;
	std::condition_variable Finished;
	std::mutex outputMutex;
	int* fileAccess = 0;
	int* totalFilesEach = 0;
	std::set<int> fileNumbers;

	ThreadInfo()
	{
		OutputTuples = 0;
		b = 0;
	}

	~ThreadInfo()
	{
		//if(b->results.size()>0) delete b;
	}
};

#endif
