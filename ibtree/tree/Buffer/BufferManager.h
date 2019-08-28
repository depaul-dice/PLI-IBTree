#ifndef BufferManager_h
#define BufferManafer_h

#include "Types/WritingBuffer.h"
#include "../Threads/ThreadManager.h"
#include "../tools/StringFunctions.h"
#include "../PLITypes/Bucket/Bucket.h"
#include "../IBTree/Types/ListBuckets.h"
#include <fstream>


class BufferManager
{
public:
	std::string dataBase;
	WritingBuffer* mainWrittingBuffer=0;
	WritingBuffer* writtingBuffer=0;
	std::condition_variable conwr;
	bool writing=false;
	ThreadManager* threadManager=0;
	ListBuckets* _outputBucket=0;
	int size = 0;
	std::string bufferMetaData = "bufferMD.dat";

	BufferManager(std::string Database, ThreadManager* ThreadManager);
	Interval AddWritingBucket(Bucket* bu);
	void save();
	void read();
};


#endif

