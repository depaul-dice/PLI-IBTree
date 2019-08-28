#include "RecordRec.h"


//This function initializes a new recordRec object.
recordRec::recordRec()
{
	selEntry = new IBEntry();
	newNode = new IBNode();
}

//This function initializes a new recordRec object with the given parameters.
recordRec::recordRec(IBEntry* SelEntry, IBNode* NewNode)
{
	selEntry = SelEntry;
	newNode = NewNode;
}