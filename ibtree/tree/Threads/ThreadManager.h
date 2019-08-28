#ifndef IBTREE_TREE_PLITYPES_THREADCONTROLLER_H_
#define IBTREE_TREE_PLITYPES_THREADCONTROLLER_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include "../Buffer/Types/BufferInterface.h"
#include "../Buffer/Types/WritingBuffer.h"
#include "../IBPlusTree/Types/IBPlusNode.h"
#include "Types/WritingThread.h"
#include "Types/ReadingScanningThread.h"
#include "../tools/Operations.h"
#include "../PLITypes/Constants.h"
#include "../Queries/Types/ThreadInfo.h"
#include "../Queries/Types/ReturningList.h"

class ThreadManager {
public:
	int threadCount;
	std::string database;


	ThreadManager(std::string Database);
	virtual ~ThreadManager();

	int writingThreadCount=0;
	std::condition_variable writeThreadsVariance;
	std::mutex writeThreadCountMutex;

	int readingThreads=0;
	std::condition_variable readThreadsVariance;

	std::mutex outputMutex;
	std::vector<std::thread> workingThreads;

	void ReadScanFile(BufferInterface* wb, ThreadInfo** pai, int count, int* FileCount, std::condition_variable* FileCountVariance);
	void WriteFile(WritingBuffer* wb, std::string Location);
	void WaitForThreads();
};

#endif
