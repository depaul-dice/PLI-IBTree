#ifndef IBTREE_TREE_PLITYPES_QUERY_QUERYMANAGER_H_
#define IBTREE_TREE_PLITYPES_QUERY_QUERYMANAGER_H_

#include <queue>
#include "../IBTree/IBTree.h"
#include "Types/ReturningList.h"
#include "Types/ThreadInfo.h"
#include "../Threads/ThreadManager.h"
#include <iostream>
#include "../IBPlusTree/IBPlusTree.h"


class QueryManager
{
	IBPlusTree * iBPlusTree;
	int fileCount;
	std::condition_variable fileAccessVariance;
	ListBuckets mergedBuckets;
	std::mutex lockQueue;
	std::queue<ThreadInfo*> myqueue2;
	std::mutex lockState;
	bool isQuery=false;
	bool isInsert=false;
	std::mutex timerMutex;
	std::condition_variable loopNotifier;
	std::mutex lockConditionIsFinished;
	std::condition_variable ConditionIsFinished;


private:
	void loop();
	void searchScanDisk(ListBuckets* _outputBucket, ThreadInfo** pai, int count);

public:
	QueryManager(std::string dnName, IBPlusTree * IBPlusTree);
	int submit(Interval query, ReturningList* OutputTuples, int* FileAccess, int* Ft);
	int insertTuple(float key, char* Value);
};



#endif /* IBTREE_TREE_PLITYPES_QUERY_QUERYMANAGER_H_ */
