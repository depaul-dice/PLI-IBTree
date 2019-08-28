#ifndef IBTREE_TREE_PLITYPES_READTHREAD2_H_
#define IBTREE_TREE_PLITYPES_READTHREAD2_H_

#include "ThreadInterface.h"
#include "../../Buffer/Types/BufferInterface.h"
#include "../../Buffer/Types/WritingBuffer.h"
#include "../../Queries/Types/ThreadInfo.h"
#include "../../tools/FloatingPoint.h"
#include "../../Queries/Types/ReturningList.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class ReadingScanningThread : public ThreadInterface {
public:
	BufferInterface* rBuffer;
	ThreadInfo **pai;
	int count;
	std::condition_variable* isReadable;
	ReturningList** _outputTuples2;
	std::mutex* outputMutex;
	int* fileCount;
	std::condition_variable* fileCountVariance;
	int* reaingThreadCount;
	std::condition_variable* threadsVariance;


	ReadingScanningThread(BufferInterface* ReadingBuffer, ThreadInfo** Pai, int Count, std::mutex* OutputMutex, int* ReadingThreadCount/*, std::mutex* ThreadCountMutex*/, std::condition_variable* ThreadsVariance, int* FileCount/*, std::mutex* FileCountMutex*/, std::condition_variable* FileCountVariance, std::condition_variable* IsReadable);
	virtual ~ReadingScanningThread();
	void Read();
	std::thread run();
};

#endif
