#ifndef IBPlusTree_h
#define IBPlusTree_h


#include "../IBTree/IBTree.h"
#include "Types/IBPlusNode.h"
#include "../tools/generalTools.h"
#include <iostream>
#include "Types/Status.h"
#include "../PLITypes/Evaluation.h"
#include "../Queries/Types/ReturningList.h"
#include "../Threads/ThreadManager.h"
#include "../PLITypes/Data.h"

class QueryManager;

class IBPlusTree
{
public:
	IIBPlusNode * rootNode;
	Evaluation* evaluation = new Evaluation();
	ThreadManager* threadManager;
	QueryManager* queryManager;
	IBTree* iBTree;
	std::string ibPlusMetaData = "ibPlusTreeMD.dat";

	Status status;

	std::string Database;

	std::ofstream foutput2;


	IBPlusTree(std::string dnName);

	void setInfo(std::string _ibPlusMetaData);

	void setIBTree1(IBTree* _ibTree);


	//##############<Insert>##############
	int insertTuple(Data key, char* Value);


	int insertTupleRec(IIBPlusNode * curNode, Data _key, char* Value);


	//##############<Search>##############
	int search(ReturningList* _outputTuples, Interval _interval, int* FileAccess, int* Ft);


	int searchRec(const IIBPlusNode * curNode, ReturningList* _outputTuples, Interval _interval);


	//#############<Group>##############
	int group();


	int groupRec(IIBPlusNode * curNode);


	//#############<Copy IBTree Structure>##############
	int copyStructure();

	int copyStructure(IBTree* ib);

	int copyStructureRec(IIBPlusNode* curNode, IBNode* ibnode);


	//#############<Print>##############
	int numPrint = 0; //Note: this variable is only used for testing, it should be commented
	int printIBPlusTree(bool printData);


	int printInfoIBPlusTreeRec(IIBPlusNode* curNode, std::string _strLable, int _level);



	int printAllIBPlusTreeRec(IIBPlusNode* curNode, std::string _strLable, int _level);

	void save();


	//#############<Write metadata>##############
	void writeMetaData();

	void writeMetaDataRec(IIBPlusNode* curNode, std::string tab);

	void read();

	int readMetaData();

	int readMetaDataR(std::ifstream* infile, IIBPlusNode** node);

	Tuple* createTuple(Data key, char* Valuee);
	int releaseBucket(Bucket* Bucket);
};



#endif


