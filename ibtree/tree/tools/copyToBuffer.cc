#include "copyToBuffer.h"

//This function is used to copy an IB-Tree node(second parameter) with a length(fifth parameter) to a node buffer(first parameter) and insert a new bucket(third parameter) at some position(fourth parameter).
bool copyBucketToBuffer(IBNodeBuffer* buffer, IBNode* node, BucketInfo value,int position,int length)
{
	for(int i=0;i< position;i++)
	{
		buffer->bucketID[i] = node->bucketID[i];
	}
	buffer->bucketID[position] = value;
	for(int i=position + 1;i< length + 1;i++)
	{
		buffer->bucketID[i] = node->bucketID[i - 1];
	}
	return true;
}


//This function is used to copy an IB-Tree node(second parameter) with a length(fifth parameter) to a node buffer(first parameter) and insert a new Interval(third parameter) at some position(fourth parameter).
bool copyIntervalToBuffer(IBNodeBuffer* buffer, IBNode* node, Interval value, int position, int length)
{
	for (int i = 0; i< position; i++)
	{
		//buffer->interval[i] = node->interval[i];
		buffer->interval[i].start = node->interval[i].start;
		buffer->interval[i].end = node->interval[i].end;
	}
	//buffer->interval[position] = value;
	buffer->interval[position].start = value.start;
	buffer->interval[position].end = value.end;
	for (int i = position + 1; i< length + 1; i++)
	{
		//buffer->interval[i] = node->interval[i - 1];
		buffer->interval[i].start = node->interval[i-1].start;
		buffer->interval[i].end = node->interval[i-1].end;
	}
	return true;
}

//This function is used to copy an IB-Tree node(second parameter) with a length(sixth parameter) to a node buffer(first parameter) and set the max for both left(third parameter) and right(fourth parameter) children at some position(fifth parameter).
bool copyMaxToBuffer(IBNodeBuffer* buffer, IBNode* node, Data leftMax, Data rightMax, int position, int length)	//non-leaf node
{
	for (int i = 0; i < position+1; i++)
	{
		buffer->max[i] = node->max[i];
	}
	if (buffer->interval[position].end < leftMax)
	{
		buffer->max[position] = leftMax;
	}
	else
	{
		buffer->max[position] = buffer->interval[position].end;
	}
	buffer->max[position + 1] = rightMax;

	for (int i = position + 2; i < length + 2; i++)
	{
		buffer->max[i] = node->max[i - 1];
	}
	return true;
}

//This function is used to copy an IB-Tree node(second parameter) with a length(fifth parameter) to a node buffer(first parameter) and insert a new child(third parameter) at some position(fourth parameter).
bool copyPointerToBuffer(IBNodeBuffer* buffer, IBNode* node, IBNode * value, int position, int length)
{
	for (int i = 0; i< position+1; i++)
	{
		buffer->pointer[i] = node->pointer[i];
	}
	buffer->pointer[position+1] = value;
	for (int i = position + 2; i< length + 2; i++)
	{
		buffer->pointer[i] = node->pointer[i - 1];
	}
	return true;
}


//This function is used to copy an IB-Tree node(second parameter) with a length(ninth parameter) to a node buffer(first parameter) and insert a new node(seveth parameter) with some specifications at some position(eightth parameter).
//The third parameter is the bucket which it gets added at that position.
//The fourth parameter is the interval of the range of data in the bucket.
//The fifth parameter is the left childs max value
//The sixth parameter is the right childs max value
//The seventh parameter is the pointer of the child which is going to be inserted at some position(eightth parameter).
bool copyNodeToBuffer(IBNodeBuffer* buffer, IBNode* node, BucketInfo bucket, Interval interval, Data leftMax, Data rightMax, IBNode * pointer,int position,int length)
{
	copyBucketToBuffer(buffer, node, bucket, position, length);
	copyIntervalToBuffer(buffer, node, interval, position, length);
	if ((leftMax > 0.0) && (rightMax > 0.0))
	{
		copyMaxToBuffer(buffer, node, leftMax,rightMax, position, length);
	}
	else //Leaf - node
	{
		for (int i = 0; i < length+1;i++) buffer->max[i] = buffer->interval[i].end;
	}
	if (pointer) copyPointerToBuffer(buffer, node, pointer, position, length);
	return true;
}

