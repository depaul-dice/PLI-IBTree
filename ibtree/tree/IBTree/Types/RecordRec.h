#ifndef RecordRec_h
#define RecordRec_h

#include "IBEntry.h"
#include "IBNode.h"

class recordRec
{
public:
	IBEntry * selEntry = 0;
	IBNode* newNode = 0;
	recordRec();
	recordRec(IBEntry* SelEntry, IBNode* NewNode);
};

#endif