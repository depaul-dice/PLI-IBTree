#include "WritingThread.h"

//This function initializes a new WritingThread object with the given parameters
//The first argument is the buffer which has to write on the disk
//The second argument is the location of the file which has to write on
//The third argument keeps track of the number of threads that are writing on the disk
//The fourth argument is the mutex that lock the variable that holds the number of writing threads to prevent it from tearing.
//The fifth argument is a conditional variable to notify when the number of writing threads has changed.
WritingThread::WritingThread(WritingBuffer* WrittingBuffer, std::string Location, int* WritingThreadCount, std::mutex* ThreadCountMutex, std::condition_variable* ThreadsVariance)
{
	// TODO Auto-generated constructor stub
	writtingBuffer=WrittingBuffer;
	location=Location;
	writingThreadCount=WritingThreadCount;
	threadCountMutex= ThreadCountMutex;
	threadsVariance=ThreadsVariance;
}

//This function destructs a writing thread object.
WritingThread::~WritingThread()
{
}

//This function runs when the writingthread starts to run
void WritingThread::write()
{
	running = true;
	writtingBuffer->isBeingWritten = true;
	threadCountMutex->lock();
	(*writingThreadCount)++;
	(*threadCountMutex).unlock();
	FILE* foutput;

#ifdef _WIN64
	fopen_s(&foutput,(location).c_str(), "a");
#elif _WIN32 || __linux__
	foutput = fopen((location).c_str(), "a");
#endif

	fwrite((const void*)writtingBuffer->data, 1, writtingBuffer->size, foutput);
	fflush(foutput);
	fclose(foutput);
	(*threadCountMutex).lock();
	(*writingThreadCount)--;
	(*threadCountMutex).unlock();
	(*threadsVariance).notify_one();
	writtingBuffer->isBeingWritten = false;
	//if (!writtingBuffer->isBeingRead)
	{
		free(writtingBuffer->data);
		delete writtingBuffer;
		writtingBuffer=0;
	}
	running = false;
	done = true;
}

//This function starts the thread.
void WritingThread::run()
{
	std::thread t1(std::thread([this] { write(); }));
	t1.detach();
}

