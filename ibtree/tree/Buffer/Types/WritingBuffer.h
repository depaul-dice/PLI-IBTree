#ifndef WritingBuffer_h
#define WritingBuffer_h

#include "../../PLITypes/Bucket/Bucket.h"
#include "BufferInterface.h"

class WritingBuffer: public BufferInterface
{
public:
	WritingBuffer(std::string DataBase, int Capacity);
	WritingBuffer(std::string DataBase, char* data, int size);

	Interval AddBucket(Bucket* bu);
	void freeBuf();
};



#endif
