#ifndef IBTREE_TREE_PLITYPES_WRITINGTHREAD_H_
#define IBTREE_TREE_PLITYPES_WRITINGTHREAD_H_


#include "ThreadInterface.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../../Buffer/Types/WritingBuffer.h"
#include <string>

class WritingBuffer;

class WritingThread: public ThreadInterface
{
public:
	WritingBuffer* writtingBuffer;
	std::string location;
	int* writingThreadCount;
	std::mutex* threadCountMutex;
	std::condition_variable* threadsVariance;
	WritingThread(WritingBuffer* WrittingBuffer, std::string Location, int* WritingThreadCount, std::mutex * ThreadCountMutex, std::condition_variable * ThreadsVariance);
	virtual ~WritingThread();
	void run();
	void write();
};

#endif /* IBTREE_TREE_PLITYPES_WRITINGTHREAD_H_ */
