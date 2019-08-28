
#include "BufferInterface.h"
#include <vector>


BufferInterface::BufferInterface()
{
}

BufferInterface::~BufferInterface()
{
}

//This function instantiates a new instance of a BufferInterface class with the given parameters
//The first argument is the name of the database.
//The second argument is the file number which the Buffer Interface points to in the database.
BufferInterface::BufferInterface(std::string DataBase, int FNumber)
{
	dataBase=DataBase;
	type=0;
	fNumber=FNumber;
}
