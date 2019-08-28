#include "PLIDataType.h"

//This function initializes the clustered section of PLI
clusteredData::clusteredData()
{
	interval = (Interval*) malloc(Constants::MAX_SIZE_CLUSTERED * 2* (sizeof(float)));
	bucketID = (float*)calloc(Constants::MAX_SIZE_CLUSTERED, sizeof(float));

	for (int i = 0; i < Constants::MAX_SIZE_CLUSTERED; i++)
	{
		interval[i].start = Constants::MAX_DISTANCE;
		interval[i].end = Constants::MIN_DISTANCE;
	}
}

//This function is to destruct the clustered section of PLI
clusteredData::~clusteredData()
{
	delete interval;
	delete bucketID;
}

//This function initializes the overflow section of PLI
overflowData::overflowData()
{
	pointer = 0;
	max = Constants::MAX_DISTANCE;
	min = Constants::MIN_DISTANCE;
}


