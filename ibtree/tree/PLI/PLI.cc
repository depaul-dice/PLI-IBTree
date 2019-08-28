#include "PLI.h"

//This function instantiates the PLI system
PLI::PLI()
{
	//The clustered part
	clustered = new clusteredData();
	countC = 0;
	indexC = 0;
	//The oveflow part
	overflow = overflowData();
	countO = 0;
	database = "PLIdata.dat";
	offset = 0;
	metadata = "PLImeta.dat";
}

//Thid function is used to release the resources that PLI uses
PLI::~PLI()
{
	delete clustered;
}

//This function sets the location of the file containing the database and the metadata
void PLI::setFile(std::string databaseFile, std::string metadataFile)
{
	database = databaseFile;
	metadata = metadataFile;
}

//This function opens the file that the database file gets written to.
void PLI::OpenDatabase()
{
	outFile.open(database, std::ofstream::out | std::ofstream::app);
}

//This function closes the file that the database file gets written to.
void PLI::CloseDatabase()
{
	outFile.close();
}


//This function is to insert a tuple in the PLI system.with the given key and value.
int PLI::insertTuple(const float & key, const std::string & data)
{
	int result = Constants::ERROR1;
	if (countC < Constants::MAX_SIZE_CLUSTERED)
	{
		//1. Append key value to PLI
		indexC += 1;
		if (clustered->interval[countC].start > key)
		{
			clustered->interval[countC].start = key;
		}
		if (clustered->interval[countC].end < key)
		{
			clustered->interval[countC].end = key;
		}
		if (indexC == Constants::NUM_ROW_PER_BUCKET)
		{
			offset = outFile.tellp();
			countC += 1;
			indexC = 0;
			clustered->bucketID[countC] = offset;
		}
		//2. Write data to database
		std::stringstream strData;
		strData << key << "\t" << data << "\n";
		outFile << strData.rdbuf();
		result = Constants::SUCCESS;
	}
	else result = insertTupleOverflow(key, data);
	return result;
}


//This function inserts a tuple to the overflow section of PLI.
int PLI::insertTupleOverflow(const float & key, const std::string & data)
{
	if (countO == 0)
	{
		offset = outFile.tellp();
		overflow.pointer = outFile.tellp();
	}
	countO += 1;

	//Sets the minimum and maximum of the keys of tuples contained in the overflow section.
	if (overflow.min > key) overflow.min = key;
	if (overflow.max < key) overflow.max = key;

	//Writes thw tuple to disk in the file containing the data.
	std::stringstream strData;
	strData << key << "\t" << data << "\n";
	outFile << strData.rdbuf();
	return Constants::OVERFLOW2;
}



//This function finds the buckets that intersect with interval out of PLI.
//The first parameter is a reference to the list of offsets where the found buckets are going to be stored in it.
int PLI::searchBucketIDs(std::vector<std::streampos>& _output, Interval _interval)
{
	int result = Constants::SUCCESS;
	if (countO > 0) _output.push_back(overflow.pointer);
	for (int i = 0; i < countC; i++)
	{
		if (intersect(_interval, clustered->interval[i])) _output.push_back(clustered->bucketID[i]);
	}
	return result;
}

//This function seeks for tuples which their key intersects with the given interval.
//The first parameter is a reference to the list of the found tuples.
int PLI::searchTuples(std::vector<long>& _output, Interval _interval)
{
	//Return all bucketIDs whose intervals intersect with a given interval
	int result = Constants::SUCCESS;
	std::ifstream inFile(database);
	//Scan in clustered part
	for (int i = 0; i < countC; i++)
	{
		if (intersect(_interval, clustered->interval[i]))
		{
			inFile.seekg(clustered->bucketID[i]);
			int numEntry = Constants::NUM_ROW_PER_BUCKET;
			if (i == countC - 1) numEntry = indexC;
			for (int j = 0; j < numEntry; j++)
			{
				std::string line;
				std::getline(inFile, line);
				if (line == "") break;
				std::string value = StringFunctions::LeftTrim(line, '\n');
				std::vector<std::string> valuesw= StringFunctions::Split(value, '\n', 2);
				std::vector<std::string> valuesw2= StringFunctions::Split(valuesw[1], '\n', 2);
				std::istringstream iss1(valuesw[0]);
				float key;
				iss1 >> key;
				std::istringstream iss2(valuesw2[0]);
				int pos;
				iss2 >> pos;
				if (inside(_interval, key)) _output.push_back(pos);
			}
		}
	}
	//Scan in the overflow page
	if (countO > 0)
	{
		inFile.seekg(overflow.pointer);
		std::string line;
		while (std::getline(inFile, line))
		{
			std::string value = StringFunctions::LeftTrim(line, '\n');
			std::vector<std::string> values = StringFunctions::Split(value, 'n', 2);
			if (values.size() >= 2)
			{
				std::istringstream iss1(values[0]);
				float key;
				iss1 >> key;
				if (inside(_interval, key))
				{
					std::istringstream iss2(values[1]);
					int pos;
					iss2 >> pos;
					_output.push_back(pos);
				}
			}
		}
	}
	inFile.close();
	return result;
}


//This function write the state of PLI as its metadat on the disk.
void PLI::writeMetaData()
{
	std::ofstream outFile(metadata);
	//format:
	// Cluster: (number of buckets in clustered part)
	// Bucket : n  size    Interval    Pointer
	// Overflow : (number of tuples in overflow page)   pointer
	if (countC > 0) outFile << "Cluster:\t" << countC << "\n";
	for (int i = 0; i < countC; i++)
	{
		outFile << "Bucket:\t" << i << "\t";
		if (i < countC - 1) outFile << "1000" << "\t";
		else outFile << indexC << "\t";
		outFile << clustered->interval[i].start << "\t" << clustered->interval[i].end << "\t" << clustered->bucketID[i] << "\n";
	}
	if (countO > 0)
	{
		outFile << "Overflow:\t" << countO << "\t";
		outFile << overflow.pointer << "\n";
	}
	outFile.flush();
	outFile.close();
}



//This function restores the state of PLI out of its metadata which is written on the disk when it starts.
void PLI::readMetaData()
{
	std::ifstream infile(metadata);
	std::string line;
	while (std::getline(infile, line))
	{
		std::vector<std::string> values= StringFunctions::Split(line, '\t');
		std::stringstream iss1(values[1]);
		if (values[0] == "Cluster:")
		{
			iss1 >> countC;
		}
		else
		{
			if (values[0] == "Bucket:")
			{
				int bkIndex;
				iss1 >> bkIndex;
				std::stringstream iss2(values[2]);
				iss2 >> clustered->interval[bkIndex].start;
				std::stringstream iss3(values[3]);
				iss3 >> clustered->interval[bkIndex].end;
				std::stringstream iss4(values[4]);
				iss4 >> clustered->bucketID[bkIndex];
			}
			else
			{
				if (values[0] == "Overflow:")
				{
					iss1 >> countO;
					std::stringstream iss2(values[2]);
					long p;
					iss2 >> p;
					overflow.pointer = p;
					//Not necessary to use min & max, we always read overflow
				}
			}
		}
	}
	infile.close();
}
