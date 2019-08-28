#ifndef copyFromBuffer_h
#define copyFromBuffer_h

#include "../PLITypes/Constants.h"
#include "../IBTree/Types/IBNode.h"
#include "../IBTree/Types/IBEntry.h"


extern "C" bool copyBucketFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start,int end);


extern "C" bool copyIntervalFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end);


extern "C" bool copyMaxFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end);


extern "C" bool copyPointerFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end);


extern "C" void copyEntryFromBuffer(IBNodeBuffer* buffer, IBEntry* entry, int position);


extern "C" void copyNodeFromBuffer(IBNodeBuffer* buffer, IBNode* node, int start, int end);

#endif
