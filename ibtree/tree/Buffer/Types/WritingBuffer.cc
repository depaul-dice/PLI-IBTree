#include "WritingBuffer.h"


//This function is to initialize an instance of a writing buffer
//The first parameter is where the writing buffer should be saved after it gets full.
//The second parameter is the maximum capacity of the writing buffer which resides in memory.
WritingBuffer::WritingBuffer(std::string DataBase, int Capacity)
{
	dataBase = DataBase;
	data=(char*) malloc(Capacity);
	size = 0;
	type=1;
}

//This function is to initialize an instance of a writing buffer with already alocated space in memory that is filled with data.
//The first parameter is the address of the space in memory which is filled with data.
//The second parameter is the size of the space in memory that the writing buffer has to point to.
WritingBuffer::WritingBuffer(std::string DataBase, char* Data, int Size)
{
	dataBase = DataBase;
	data=Data;
	size=Size;
	type=1;
}


//This function adds a bucket conaining its data to the writing buffer
Interval WritingBuffer::AddBucket(Bucket* bu)
{
	bu->bucketID.size = bu->length + sizeof(int)*Constants::NUM_ROW_PER_BUCKET;
	bu->bucketID.offset = size;
	Interval interval;
	interval.start=Constants::MAX_DISTANCE;
	interval.end=Constants::MIN_DISTANCE;
	int l = 0;
	for (int i = 0; i<Constants::NUM_ROW_PER_BUCKET; i++)
	{
		if (interval.start > bu->tuples[i]->key)
		{
			interval.start = bu->tuples[i]->key;
		}
		if (interval.end < bu->tuples[i]->key)
		{
			interval.end = bu->tuples[i]->key;
		}
		memcpy(data+ size, (void*)&bu->tuples[i]->length, sizeof(int));
		size += sizeof(int);
		memcpy(data+ size, bu->tuples[i]->value, bu->tuples[i]->length);
		size += bu->tuples[i]->length;
		if(!isBeingRead)
		{
		delete bu->tuples[i];
		}
	}
	return interval;
}

//This function deallocates the memory owned bu the writing buffer 
void WritingBuffer::freeBuf()
{
	free(data);
}
