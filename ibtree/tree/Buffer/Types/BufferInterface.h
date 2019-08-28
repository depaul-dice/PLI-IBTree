#ifndef BufferInterface_h
#define BufferInterface_h

#include "../../IBTree/Types/BucketTree.h"
#include <vector>
#include <string>

class BufferInterface
{
public:
	char* data;
	int type=0;
	bool isBeingWritten = false;
	bool isBeingRead= false;
	int size = 0;
	std::string dataBase;
	int fNumber = 1;
	std::vector<BucketTree> readBucks;


	BufferInterface();
	~BufferInterface();
	BufferInterface(std::string DataBase, int fNumber);
};



#endif
