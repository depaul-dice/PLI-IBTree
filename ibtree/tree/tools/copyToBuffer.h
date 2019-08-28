#ifndef copyToBuffer_h
#define copyToBuffer_h


#include "../PLITypes/Constants.h"
#include "../PLITypes/Interval.h"
#include "../IBTree/Types/IBNode.h"
#include "../PLITypes/Bucket/Types/BucketInfo.h"
#include "../PLITypes/Data.h"


bool copyBucketToBuffer(IBNodeBuffer* buffer, IBNode* node, BucketInfo value,int position,int length);


bool copyIntervalToBuffer(IBNodeBuffer* buffer, IBNode* node, Interval value, int position, int length);


bool copyMaxToBuffer(IBNodeBuffer* buffer, IBNode* node, Data curMax, Data newMax, int position, int length);


bool copyPointerToBuffer(IBNodeBuffer* buffer, IBNode* node, IBNode * value, int position, int length);



bool copyNodeToBuffer(IBNodeBuffer* buffer, IBNode* node, BucketInfo bucket, Interval interval, Data curMax, Data newMax, IBNode * pointer,int position,int length);

#endif
