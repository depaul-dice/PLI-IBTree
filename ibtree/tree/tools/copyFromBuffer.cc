#include "copyFromBuffer.h"


//This function is used to copy buckets contained in the node buffer(first parameter) to a real node(second parameter) from an index(third parameter) to some another index(fourth parameter)
bool copyBucketFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start,int end)	//[start, end]
{
	int index = 0;
	for (int i = start; i < end+1; i++)
	{
		node->bucketID[index] = buffer->bucketID[i];
		index += 1;
	}
	return true;
}


//This function is used to copy intervals contained in the node buffer(first parameter) to a real node(second parameter) from an index(third parameter) to some another index(fourth parameter)
bool copyIntervalFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end)	//[start, end]
{
	int index = 0;
	for (int i = start; i < end+1; i++)
	{
		node->interval[index]= buffer->interval[i];
		index += 1;
	}
	return true;
}

//This function is used to copy max values of the children contained in the node buffer(first parameter) to a real node(second parameter) from an index(third parameter) to some another index(fourth parameter)
bool copyMaxFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end)
{
	int index = 0;
	for (int i = start; i < end+1; i++)
	{
		node->max[index] = buffer->max[i];
		//print i, range(start, end + 1)
		index += 1;
	}
	return true;
}

//This function is used to copy child pointers contained in the node buffer(first parameter) to a real node(second parameter) from an index(third parameter) to some another index(fourth parameter)
bool copyPointerFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end)
{
	int index = 0;
	for (int i = start; i < end+1; i++)
	{
		node->pointer[index] = buffer->pointer[i];
		index += 1;
	}
	return true;
}

//This function is used to copy some data from some information at a specific position from a buffers elements to an IBEntry object
void copyEntryFromBuffer(IBNodeBuffer* buffer, IBEntry* entry, int position)
{
	entry->interval= buffer->interval[position];
	entry->max = buffer->max[position];
	entry->bucketID = buffer->bucketID[position];
}

//This function is used to copy the elements and values contained in the node buffer(first parameter) to a real node(second parameter) from an index(third parameter) to some another index(fourth parameter)
void copyNodeFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end)
{
	copyBucketFromBuffer(buffer, node, start, end);
	copyIntervalFromBuffer(buffer, node, start, end);
	copyMaxFromBuffer(buffer, node, start, end + 1);
	copyPointerFromBuffer(buffer, node, start, end + 1);
}
