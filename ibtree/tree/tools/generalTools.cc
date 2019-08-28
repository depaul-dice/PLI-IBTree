#include "generalTools.h"


//This function operates based on whether the given node is a leaf or not.
//If its a leaf, it will find a bucket where a tuple with the given key could be added to.
//If its not a leaf, it will find a child where one of its descendant buckets could own a tuple woth the specified key.
int findGoodPlace(IIBPlusNode * node, Data key, bool isLeaf)
{
	int goodPlace;
	if (!node) return -1;
	if (isLeaf) goodPlace = node->length - 1;
	else
	{
		goodPlace = node->length;
	}
	for (int i = 0; i < node->length; i++)
	{
		if (key <= node->interval[i].start)
		{
			goodPlace = i;
			break;
		}
	}
	return goodPlace;
}

