#include "Bucket.h"

//This function initializes a new bucket.
Bucket::Bucket()
{
	bucketID.offset = 0;
	Count = 0;
}


//This function adds a tuple to a bucket object.
void Bucket::AddTuple(Tuple* tuple)
{
	tuples[Count] = tuple;
	length += strlen(tuple->value);
	Count++;
}