#ifndef PLI_h
#define PLI_h


#include "Types/PLIDataType.h"
#include "../tools/Operations.h"
#include "../PLITypes/Constants.h"
#include "../tools/StringFunctions.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

class PLI
{
public:
	clusteredData * clustered;
	overflowData overflow = overflowData();
	int indexC = 0;
	int countC = 0;
	int countO = 0;
	std::string database = "PLIdata.dat";
	std::string metadata = "PLImeta.dat";
	long offset = 0;
	std::ofstream outFile;


	PLI();
	~PLI();

	void setFile(std::string databaseFile, std::string metadataFile);
	void OpenDatabase();
	void CloseDatabase();

	//#############<Insert>##############
	int insertTuple(const float & key, const std::string & data);
	int insertTupleOverflow(const float & key, const std::string & data);


	//#############<Search>##############
	int searchBucketIDs(std::vector<std::streampos>& _output, Interval _interval);
	int searchTuples(std::vector<long> & _output, Interval _interval);

	void writeMetaData();
	void readMetaData();
};


#endif
