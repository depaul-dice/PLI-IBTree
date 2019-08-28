#include "BufferManager.h"



//This function instantiates the buffer manager with the first parameter and the pointer to thread manager as the second parameter.
BufferManager::BufferManager(std::string Database, ThreadManager* ThreadManager)
{
	dataBase = Database;
	_outputBucket = new ListBuckets();

	//This function creates a buffer for the buckets which are ready to  be flushed on the disk with the destination of where it has to be saved givven by the first parameter and its maximum capacity in the memory.
	mainWrittingBuffer =new WritingBuffer(dataBase,Constants::BufferCapacity);
	threadManager=ThreadManager;
	//fileCount=0;
}

//This function adds the bucket to the IB-Tree and add its content to the buffer.
Interval BufferManager::AddWritingBucket(Bucket* bu)
{
	bu->bucketID.fNum = mainWrittingBuffer->fNumber;

	//Add the buckets contents to the buffer
	Interval interval=mainWrittingBuffer->AddBucket(bu);

	//Now we will check if the buffer has reached to the limit which in that case has to be written on the disk.
	if (mainWrittingBuffer->size >= Constants::MaxFileSize)
	{
		std::string s = "./" + dataBase + "/" + dataBase + "-" + std::to_string(mainWrittingBuffer->fNumber);

		char* b=(char*) malloc(mainWrittingBuffer->size);
		memcpy(b, mainWrittingBuffer->data,mainWrittingBuffer->size);
		writtingBuffer =new WritingBuffer(dataBase,b, mainWrittingBuffer->size);
		writtingBuffer->fNumber=mainWrittingBuffer->fNumber;
		threadManager->WriteFile(writtingBuffer,s);
		mainWrittingBuffer->fNumber++;
		mainWrittingBuffer->size = 0;
	}
	return interval;
}

//This function saves the status of the buffer manager on the disk as a metadata before the application closes.
void BufferManager::save()
{
	std::ofstream* fout = new std::ofstream(dataBase + "/" + bufferMetaData, std::ofstream::out);
	(*fout) << std::to_string( mainWrittingBuffer->fNumber)+","+ std::to_string(mainWrittingBuffer->size)+"\n";
	(fout)->write( mainWrittingBuffer->data, mainWrittingBuffer->size);
	fout->close();
	threadManager->WaitForThreads();
}


//This function restores the state of the buffer manager from the file that contained the metadata
void BufferManager::read()
{
	std::string STRING;
	std::ifstream infile;
	infile.open(dataBase + "/" + bufferMetaData, std::ofstream::in);
	getline(infile, STRING);
	std::vector<std::string> values = StringFunctions::Split(STRING, ',');
	mainWrittingBuffer->fNumber = std::stoi(values[0]);
	mainWrittingBuffer->size = std::stoi(values[1]);
	char *buffer = new char[mainWrittingBuffer->size];
	infile.read(buffer, mainWrittingBuffer->size);
	infile.close();
	memcpy(mainWrittingBuffer->data, buffer, mainWrittingBuffer->size);
	free(buffer);
}




