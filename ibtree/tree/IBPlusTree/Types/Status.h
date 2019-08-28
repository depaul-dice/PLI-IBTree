#ifndef Status_h
#define Status_h

#include "../../PLITypes/Constants.h"

struct Status
{
	long long dataArea = Constants::MAX_DATA_BUFFER;
	long long flushingArea = Constants::MAX_FLUSHING_BUFFER;
	long long totalSize = dataArea + flushingArea;
	long long availableDA = dataArea * 1024 * 2048;
	long long usedDA = 0;
	long long usedFA = 0;
	long long availableFA = flushingArea * 1024 * 2048;

	Status();
};

#endif