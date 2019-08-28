#include "ThreadManager.h"


ThreadManager::ThreadManager(std::string Database)
{
	database=Database;
	writingThreadCount=0;
	readingThreads=0;
}

ThreadManager::~ThreadManager()
{
}





void ThreadManager::ReadScanFile(BufferInterface* buf, ThreadInfo** pai, int count, int* FileCount/*, std::mutex* FileCountMutex*/, std::condition_variable* FileCountVariance)
{
	outputMutex.lock();
	readingThreads++;
	ReadingScanningThread* w = new ReadingScanningThread(buf, pai, count, &outputMutex, &readingThreads/*, &readThreadCountMutex*/, &readThreadsVariance, FileCount/*, FileCountMutex*/, FileCountVariance, &writeThreadsVariance);
	w->run();
	outputMutex.unlock();
}





void ThreadManager::WriteFile(WritingBuffer* wb, std::string Location)
{
	std::mutex mtx4;
	if (writingThreadCount>0)
	{
		std::unique_lock<std::mutex> lck(mtx4);
		writeThreadsVariance.wait(lck);
	}
	WritingThread* w=new WritingThread(wb,Location, &writingThreadCount,&writeThreadCountMutex,&writeThreadsVariance);
	w->run();
}

std::mutex mtx4;
void ThreadManager::WaitForThreads()
{
	outputMutex.lock();
	int numthreads=readingThreads;
	outputMutex.unlock();
	while (numthreads>0)
	{
		//std::cout << "wcount1: " << numthreads << "\n\n";
		std::unique_lock<std::mutex> lck(mtx4);
		readThreadsVariance.wait(lck, [this](){
			outputMutex.lock();
			//std::cout << "wcount2: " << readingThreads << "\n\n";
			int numhreads=readingThreads;
			outputMutex.unlock();
			return numhreads == 0; });

		outputMutex.lock();
		numthreads=readingThreads;
		outputMutex.unlock();
	}
}




