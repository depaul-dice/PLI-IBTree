#ifndef Evaluation_h
#define Evaluation_h

#include "Constants.h"
#include "Interval.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class Evaluation
{
public:
	int scannedEntries = 0;
	int returnBuckets = 0;
	Data DBMIN = Constants::MAX_DISTANCE;
	Data DBMAX = Constants::MIN_DISTANCE;
	Data IntervalLow = 0.0;
	Data IntervalHigh = 0.0;
	int totalBuckets = 0;
	Evaluation();

	int countSE(int num = 1);

	int countReturnBuckets(int num = 1);

	int countTotalBuckets(int num = 1);

	int setInterval(Interval interval);

	void setDBInterval(Interval interval);

	int getSE();

	int getReturnBuckets();

	int printEvalInfo();

	int printEvalInfoToFile(std::string filename);
};

#endif