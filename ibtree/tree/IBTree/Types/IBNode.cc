#include "IBNode.h"

	//This function is to initialize an IB-Tree node
	IBNode::IBNode()
	{
		int level = 0;
		int length = 0;
		bool isLeaf = true;

		//Entries
		for (int i = 0; i < Constants::MAX_NUM_ENTRY; i++)
		{
			interval[i].start=0.0;
			interval[i].end=0.0;
		}
		std::fill(max, max+(Constants::MAX_NUM_ENTRY+1), 0.0);
	}



	//This function is to initialize an IB-Tree node buffer
	IBNodeBuffer::IBNodeBuffer()
	{
		int level = 0;
		int length = 0;
		bool isLeaf = true;

		//Entries
		for (int i = 0; i < Constants::MAX_NUM_ENTRY+1; i++)
		{
			interval[i].start=0.0;
			interval[i].end=0.0;
		}

		std::fill(max, max+(Constants::MAX_NUM_ENTRY+1+1), 0.0);
	}
