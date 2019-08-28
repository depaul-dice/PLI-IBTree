#ifndef IBTree_h
#define IBTree_h

#include "Types/IBNode.h"
#include "Types/IBEntry.h"
#include "Types/RecordMax.h"
#include "Types/RecordRec.h"
#include "Types/ListBuckets.h"
#include "../Buffer/BufferManager.h"
#include "../tools/copyToBuffer.h"
#include "../tools/copyFromBuffer.h"
#include "../PLITypes/Interval.h"
#include "../PLITypes/Constants.h"
#include "../PLITypes/Evaluation.h"

class ThreadManager;

class IBTree
{
public:
	BufferManager * bufferManager;
	IBNode* rootNode = new IBNode();
	Evaluation evaluation = Evaluation();
	int numPrint = 0;
	std::string ibTreeMetaData = "ibTreeMD.dat";
	std::string Database;

	IBTree(std::string dnName, ThreadManager* threadManager);

	int insertBucket(Bucket* _bucketID);

	int insertBucket(Interval _interval, BucketInfo _bucketID);


	int insertBucketRec(IBNode* curNode, Interval _interval, BucketInfo _bucketID, recordRec* recRec, recordMax* recMax);


	int search(ListBuckets* _output, Interval _interval);


	int searchRec(IBNode* curNode, ListBuckets* _output, Interval _interval);


	//#############<Print>##############
	int printIBTree();


	int printIBTreeRec(IBNode* curNode, std::string _strLable, int _level);


	IBNode* getRootNode();

	int getSE();

	int printEvalInfo();

	void writeMetaData();

	void writeMetaDataRec(IBNode* curNode, std::string tab);

	int readMetaData();

	int readMetaDataRec(std::ifstream* infile, IBNode** node);
};


#endif
