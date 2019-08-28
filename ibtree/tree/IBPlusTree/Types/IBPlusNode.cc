#include "IBPlusNode.h"




	
	IIBPlusNode::~IIBPlusNode()
	{

	}


	//This function is used to initialize an IB+-Tree internal node
	IBPlusNLNode::IBPlusNLNode()
	{
		int level = 0;
		int length = 0;
		isLeaf=false;

		//Entries
		for (int i = 0; i < Constants::MAX_NUM_NL_ENTRY; i++)
		{
			interval[i].start=0.0;
			interval[i].end=0.0;
		}

		for(int i=0;i<Constants::MAX_NUM_NL_ENTRY+1;i++)
		{
			max[i]=0.0;
		}
	}


	//This function is used to initialize an IB+-Tree leaf node
	IBPlusLNode::IBPlusLNode()
	{
		int level = 0;
		int length = 0;
		isLeaf=true;

		//Entries
		for (int i = 0; i < Constants::MAX_NUM_L_ENTRY; i++)
		{
			interval[i].start=0.0;
			interval[i].end=0.0;
		}


		for (int i = 0; i < Constants::MAX_NUM_L_ENTRY; i++)
		{
			data[i] = 0;
		}


		for(int i=0;i<Constants::MAX_NUM_L_ENTRY;i++)
		{
			count[i]=0;
		}
	}



	//This function is used to create a new IB+-Tree node
	//The parameter is to determine whether the node should be a leaf node or not.
	IIBPlusNode* newIBPlusNode(bool isLeaf)
	{
		if (isLeaf)
		{
			return new IBPlusLNode();
		}
		else
		{
			return new IBPlusNLNode();
		}
	}
