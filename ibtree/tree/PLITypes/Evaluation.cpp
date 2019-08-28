#include "Evaluation.h"


//This function is used to initialize an evaluation object.
Evaluation::Evaluation()
{
	scannedEntries = 0;
	returnBuckets = 0;
	DBMIN = Constants::MAX_DISTANCE;
	DBMAX = Constants::MIN_DISTANCE;
	IntervalLow = 0.0;
	IntervalHigh = 0.0;
	totalBuckets = 0;
}

//This function is used to increment the number of scanned buckets in the evaluation object.
int Evaluation::countSE(int num)
{
	scannedEntries += num;
	return Constants::SUCCESS;
}

//This function is used to increment the number of returned buckets in the evaluation object.
int Evaluation::countReturnBuckets(int num)
{
	returnBuckets += num;
	return Constants::SUCCESS;
}

//This function is used to increment the total number buckets in the evaluation object.
int Evaluation::countTotalBuckets(int num)
{
	totalBuckets += num;
	return Constants::SUCCESS;
}

//This function is to record the last interval that the database was searched up on.
int Evaluation::setInterval(Interval interval)
{
	IntervalLow = interval.start;
	IntervalHigh = interval.end;
	return Constants::SUCCESS;
}

//This function is to record the minimum and maximum value that the database was queried up on.
void Evaluation::setDBInterval(Interval interval)
{
	if (DBMAX < interval.end) DBMAX = interval.end;
	if (DBMIN > interval.start) DBMIN = interval.start;
}

//This function returns the number of scanned buckets.
int Evaluation::getSE()
{
	return scannedEntries;
}

//This function returns the number of total buckets.
int Evaluation::getReturnBuckets()
{
	return returnBuckets;
}

//This function prints the records on to the console.
int Evaluation::printEvalInfo()
{
	printf("===============<Evaluation Information>==================\n");
	printf("Total number of buckets in database: %i\n", totalBuckets);
	printf("MIN value: %f; MAX value: %f\n", DBMIN, DBMAX);
	printf("Interval: [%f, %f]\n", IntervalLow, IntervalHigh);
	printf("Scanned Buckets: %i\n", scannedEntries);
	printf("Results: %i buckets\n", returnBuckets);
	printf("==========================<End>==========================\n");
	return Constants::SUCCESS;
}

//This function prints the records in a file with the path of it s parameter.
int Evaluation::printEvalInfoToFile(std::string filename)
{
	//FILE * pFile;
	//pFile = fopen(filename, "a+");
	std::ostringstream strData;
	strData << "===============<Evaluation Information>==================\n";
	strData << "Total number of buckets in database: " << totalBuckets << "\n";
	strData << "MIN value: " << DBMIN << "; MAX value: " << DBMAX << "\n";
	strData << "Interval: [" << IntervalLow << ", " << IntervalHigh << "]" << "\n";
	strData << "Scanned Buckets: " << scannedEntries << "\n";
	strData << "Results: " << returnBuckets << " buckets\n";
	strData << "==========================<End>==========================\n";
	std::ofstream outFile(filename);
	outFile << strData.rdbuf();
	outFile.close();
	return Constants::SUCCESS;
}