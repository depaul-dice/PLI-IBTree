#ifndef IIBPlusNode_h
#define IIBPlusNode_h


#include "../../PLITypes/Constants.h"
#include "../../PLITypes/Bucket/Bucket.h"
#include "../../PLITypes/Interval.h"
#include "../../PLITypes/Data.h"


class IIBPlusNode
{
public:
	int level = 0;
	int length = 0;
	Interval interval[Constants::MAX_NUM_NL_ENTRY];
	bool isLeaf;
	~IIBPlusNode();
};



//Non-Leaf node
class IBPlusNLNode: public IIBPlusNode
{
public:
	//Entries
	Data max[Constants::MAX_NUM_NL_ENTRY + 1];

	//Pointers
	IIBPlusNode * pointer[Constants::MAX_NUM_NL_ENTRY+1];


	IBPlusNLNode();
};


//Leaf node
class IBPlusLNode: public IIBPlusNode
{
public:
	IBPlusLNode* sibling=0;

	//Entries
	Bucket* data[Constants::MAX_NUM_L_ENTRY];
	int count[Constants::MAX_NUM_L_ENTRY];


	IBPlusLNode();
};

IIBPlusNode* newIBPlusNode(bool isLeaf);

#endif




