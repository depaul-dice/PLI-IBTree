#include "Status.h"


//This function initializes a new status object.
Status::Status()
{
	dataArea = Constants::MAX_DATA_BUFFER;
	flushingArea = Constants::MAX_FLUSHING_BUFFER;
	totalSize = dataArea + flushingArea;
	availableDA = dataArea * 1024 * 2048;
	usedDA = 0;
	availableFA = flushingArea * 1024 * 2048;
	usedFA = 0;
}