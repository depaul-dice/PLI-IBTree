#include "IBPlusTree.h"
#include "../Queries/QueryManager.h"



//This function instantiates the IB+-Tree of the system
IBPlusTree::IBPlusTree(std::string dnName)
{
	rootNode = 0;
	evaluation = new Evaluation();
	threadManager = new ThreadManager(dnName);
	iBTree = new IBTree(dnName, threadManager);
	queryManager = new QueryManager(dnName, this);
	Database= dnName;
}


//This fuction set the string of the file were the metadata of the IB+-Tree is saved.
void IBPlusTree::setInfo(std::string _ibPlusMetaData)
{
	ibPlusMetaData = _ibPlusMetaData;
}

void IBPlusTree::setIBTree1(IBTree* _ibTree)
{
	iBTree = _ibTree;
}


//This function is to insert a key with its value.
int IBPlusTree::insertTuple(Data key, char* Value)
{
	int result = Constants::ERROR1;
	if (rootNode == 0)
	{
		std::cout << "IB+-Tree is Null! \nWe recommend the users to build IB+-Tree structure by calling copyStructure(IBTree) before inserting data into the tree! \nAre you sure to insert data into an Empty IB+-Tree? (y/n)";
		std::string str = "";
		std::getline(std::cin, str);
		if (str == "y") rootNode = newIBPlusNode(true);
		else return result;
	}
	result = insertTupleRec(rootNode, key, Value);
	return result;
}


//This function is to finds the approprite child to go down the tree and add  the key with its value
int IBPlusTree::insertTupleRec(IIBPlusNode * curNode, Data _key, char* Value)
{
	//Recursively insert a tuple into IBPlus-Tree
	int result = Constants::ERROR1;
	if (curNode == 0)
	{
		//This is if the current node pointer does not point to a real object.
		std::cout << "####################<Error code: 10>#######################";
		return result;
	}

	//This sentence finds a child to go down the tree or the correct bucket to insert the tuple to it.
	int goodPlace = findGoodPlace(curNode, _key, curNode->isLeaf);
	if (curNode->isLeaf)
	{
		//This block is when we have reached a leaf and we have to find the bucket to insert the tuple.
		IBPlusLNode* lcurNode = (IBPlusLNode*)curNode;
		//This sentence creates tuple out of the key and its value.
		Tuple* tuple = createTuple(_key, Value);
		if (tuple == 0)
		{
			//This block runs when the used space has reached to its limitation. 
			std::cout << "####################<Warning code: 11>#######################";
			group();
			tuple = createTuple(_key, Value);
		}

		if (lcurNode->data[goodPlace] == 0)
		{
			//This block is when the bucket that the tuple which is going to be inserted in it is not initailized.
			lcurNode->data[goodPlace] = new Bucket();
			lcurNode->data[goodPlace]->length = 0;
		}
		lcurNode->data[goodPlace]->tuples[lcurNode->count[goodPlace]] = tuple;
		lcurNode->data[goodPlace]->length += tuple->length;
		lcurNode->count[goodPlace] += 1;
		result = Constants::SUCCESS;


		if (lcurNode->interval[goodPlace].end < _key)
		{
			//Update the max boundry of the bucket
			lcurNode->interval[goodPlace].end = _key;
			result = Constants::SUCCESS_UPDATE_MAX;
		}

		if (lcurNode->interval[goodPlace].start > _key)
		{
			//update the min boundry of the bucket
			lcurNode->interval[goodPlace].start = _key;
		}

		if (lcurNode->count[goodPlace] >= Constants::NUM_ROW_PER_BUCKET)
		{
			//This is when the bucket has reached to its limit of tuple and has to be flushed to disk.
			//bufferController->AddWritingBucket(lcurNode->data[goodPlace]);
			iBTree->insertBucket(lcurNode->data[goodPlace]);

			releaseBucket(lcurNode->data[goodPlace]);
			delete lcurNode->data[goodPlace];
			lcurNode->data[goodPlace] = 0;
			lcurNode->count[goodPlace] = 0;
			lcurNode->interval[goodPlace].end = curNode->interval[goodPlace].start;
			if (result == Constants::SUCCESS) result = Constants::SUCCESS_UPDATE_MAX;
		}
	}
	else
	{
		//Non-Leaf node
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		//This is a recursive call to the current function to go down the tree or to insert the tuple to the appropriate bucket.
		result = insertTupleRec(lcurNode->pointer[goodPlace], _key, Value);
		if (result == Constants::SUCCESS_UPDATE_MAX)
		{
			//This block runs when a tuple has been inserted ti a bucket that its key is higher that the current max of its ancesters. It makes their max to be updated.
			IBPlusNLNode* vv = (IBPlusNLNode*)lcurNode->pointer[goodPlace];
			for (int i = 0; i< vv->length + 1; i++)
			{
				if (lcurNode->max[goodPlace] < vv->interval[i].end)
				{
					lcurNode->max[goodPlace] = vv->interval[i].end;
				}
			}
			Data x = lcurNode->max[goodPlace];
			lcurNode->interval[goodPlace].end = x;
			result = Constants::SUCCESS_UPDATE_MAX;
		}
	}
	return result;
}


//This function is used to search the database on an interval(second parameter).
//It returns the tuples through the first parameter(ListTUples)
//The third parameter is th number of files, read and searched only to find the tuples being in the specified range.
//The fourth parameter is the total number of files, read during the search that is happening for the threads in the concurrency queue. 
int IBPlusTree::search(ReturningList* _outputTuples, Interval _interval, int* FileAccess, int* Ft)
{
	int result = 0;
	queryManager->submit(_interval, _outputTuples, FileAccess, Ft);
	searchRec(rootNode, _outputTuples, _interval);
	return result;
}


//This function is to recursively search the IB+-Tree on an interval(third parameter).
//The first parameter is the current node that the search is happening at. 
//It returns the tuples through the second parameter(ListTUples)
int IBPlusTree::searchRec(const IIBPlusNode * curNode, ReturningList* _outputTuples, Interval _interval)
{
	int result = Constants::SUCCESS;
	if (curNode == 0)
	{
		result = Constants::ERROR1;
		return result;
	}
	if (curNode->isLeaf)
	{
		IBPlusLNode* lcurNode = (IBPlusLNode*)curNode;
		for (int i = 0; i< lcurNode->length; i++)
		{
			if (lcurNode->data[i] != 0)
			{
				if (_interval.end < lcurNode->interval[i].start) break;
				if (intersect(_interval, lcurNode->interval[i]))
				{
					for (int j = 0; j< lcurNode->count[i]; j++)
					{
						if (inside(_interval, lcurNode->data[i]->tuples[j]->key))
						{
							char* str3 = (char*)malloc(lcurNode->data[i]->tuples[j]->length + 1);
							memcpy(str3, (lcurNode->data[i]->tuples[j]->value), (lcurNode->data[i]->tuples[j]->length + 1));
							_outputTuples->Add(str3);
						}
					}
				}
			}
		}
	}
	else
	{
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		for (int i = 0; i< lcurNode->length; i++)
		{
			if (result == Constants::ERROR1) return result;
			if ((_interval.start <= lcurNode->max[i]) && (lcurNode->pointer[i] != 0))
			{
				result = searchRec(lcurNode->pointer[i], _outputTuples, _interval);
			}
			if (_interval.end < lcurNode->interval[i].start)
			{
				return result;
			}
		}
		if ((_interval.start <= lcurNode->max[lcurNode->length]) && (lcurNode->pointer[curNode->length] != 0))
			result = searchRec(lcurNode->pointer[lcurNode->length], _outputTuples, _interval);
	}
	return result;
}



//#############<Group>##############
int IBPlusTree::group()
{
	//Grouping data in all leaf nodes
	// Grouping:
	//   + 1. Combining data in all entries of a leaf node to create full buckets
	//   + 2. Moving these buckets to flushing area
	//   + 3. Flush these buckets into database (Automatically be called by moveBucket(tuples) function
	int result = Constants::ERROR1;
	if (rootNode == 0) return result;
	result = groupRec(rootNode);
	return result;
}


int IBPlusTree::groupRec(IIBPlusNode * curNode)
{
	int result = Constants::ERROR1;
	if (curNode == 0) return result;
	result = Constants::SUCCESS;
	if (curNode->isLeaf)
	{
		//Leaf node
		IBPlusLNode* lcurNode = (IBPlusLNode*)curNode;
		Tuple* tuples[Constants::NUM_ROW_PER_BUCKET];
		int index = 0;
		int totalEntries = 0;
		for (int i = 0; i< lcurNode->length; i++)
		{
			totalEntries += lcurNode->count[i];
		}
		float totalBuckets = totalEntries / Constants::NUM_ROW_PER_BUCKET;
		int i = 0;

		Bucket* b = new Bucket();
		b->length = 0;


		while (totalBuckets > 0)
		{
			int length = lcurNode->count[i];
			for (int j = 0; j< length; j++)
			{
				b->tuples[index] = (lcurNode->data[i]->tuples[lcurNode->count[i] - j - 1]);
				b->length += (lcurNode->data[i]->tuples[lcurNode->count[i] - j - 1])->length;
				index += 1;
				lcurNode->data[i]->length -= lcurNode->data[i]->tuples[lcurNode->count[i] - j - 1]->length;
				lcurNode->count[i] -= 1;
				if (index == Constants::NUM_ROW_PER_BUCKET)
				{
					iBTree->bufferManager->AddWritingBucket(b);
					releaseBucket(lcurNode->data[i]);
					b->length = 0;
					index = 0;
					totalBuckets -= 1;
					if (totalBuckets == 0) break;
				}
			}
			i += 1;
		}
	}
	else
	{
		//Non-Leaf node
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		for (int i = 0; i< lcurNode->length + 1; i++)
		{
			if (result == Constants::ERROR1) break;
			result = groupRec(lcurNode->pointer[i]);
		}
	}
	return result;
}


//This function Copies the current IB-Trees Structure which is constructed based upon the history of the insertion and will be used for indexing.
int IBPlusTree::copyStructure()
{
	return copyStructure(iBTree);
}

//This function Copies an IB-Trees Structure which is constructed based upon the history of the insertion and will be used for indexing.
int IBPlusTree::copyStructure(IBTree* ib)
{
	int result = Constants::ERROR1;
	if (rootNode != 0)
	{
		//This block is  ran when the IB+-Tree has already have been made based on a previously defined structure.
		//So we have to ask the user to make sure if its previous structure should be removed or not. 
		std::cout << "IB+-Tree is not Null! All data in the IB+-Tree will be lost! Do you want to continue (y/n)?:";
		std::string str = "";
		std::getline(std::cin, str);
		if (str != "y") return result;
	}
	IBNode* IBRoot = ib->getRootNode();
	rootNode = newIBPlusNode(IBRoot->isLeaf);
	result = copyStructureRec(rootNode, IBRoot);
	//The next part is to set the smallest value for the IB+-Tree
	bool notFound = true;
	IIBPlusNode* tempNode = rootNode;
	while (notFound)
	{
		if (tempNode->isLeaf)
		{
			tempNode->interval[0].start = Constants::MIN_DISTANCE;
			notFound = false;
		}
		else
		{
			IBPlusNLNode* lcurNode = (IBPlusNLNode*)tempNode;
			tempNode = lcurNode->pointer[0];
		}
	}
	return result;
}


//This function recursively goes through each IB-Tree node and copies its structure to IB+-Tree node.
int IBPlusTree::copyStructureRec(IIBPlusNode* curNode, IBNode* ibnode)
{
	int result = Constants::ERROR1;
	if (ibnode == 0) return result;
	curNode->length = ibnode->length;
	curNode->level = ibnode->level;
	for (int i = 0; i< ibnode->length; i++)
	{
		curNode->interval[i].start = ibnode->interval[i].start;
		curNode->interval[i].end = ibnode->interval[i].start;
	}
	result = Constants::SUCCESS;
	if (ibnode->isLeaf == false)
	{
		//This block runs when the node is not a leaf node which in this case has to go the the children and copy their structure too.
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		for (int i = 0; i< ibnode->length; i++)
		{
			lcurNode->pointer[i] = newIBPlusNode(ibnode->pointer[i]->isLeaf);
			lcurNode->max[i] = ibnode->max[i];
			result = copyStructureRec(lcurNode->pointer[i], ibnode->pointer[i]);
			if (result == Constants::ERROR1) return result;
		}
		//Last child
		lcurNode->pointer[ibnode->length] = newIBPlusNode(ibnode->pointer[ibnode->length]->isLeaf);
		lcurNode->max[ibnode->length] = ibnode->max[ibnode->length];
		result = copyStructureRec(lcurNode->pointer[ibnode->length], ibnode->pointer[ibnode->length]);
	}
	return result;
}




//This function is to print and save the IB+-Trees data to the disk.
//The bool parameter is to enforce the printed data to be saved on the disk.
int IBPlusTree::printIBPlusTree(bool printData)
{
	//Opens a file to print to it.
	foutput2.open("data.dat");
	//print IB+-Tree
	int result = Constants::ERROR1;
	if (rootNode == 0) return result;
	numPrint = 0;
	if (printData) result = printAllIBPlusTreeRec(rootNode, "", 0);
	else result = printInfoIBPlusTreeRec(rootNode, "", 0);
	std::cout << "=================================== Number of elements: " << numPrint << " ===========================================\n";
	foutput2 << "=================================== Number of elements: " << numPrint << " ===========================================\n";
	foutput2.close();
	return result;
}

//This recursive function is to print a nodes data and its childrens data in the IB+-Trees data to the console.
int IBPlusTree::printInfoIBPlusTreeRec(IIBPlusNode* curNode, std::string _strLable, int _level)
{
	//print all info in IB+-Tree recursively
	int result = Constants::ERROR1;
	if (curNode == 0)
	{
		std::cout << "++++++++++++++++++++++++++++<ERROR CODE: 0>++++++++++++++++++++++++++++++++\n";
		return result;
	}
	std::string strLable = "";
	if (_strLable != "") strLable = _strLable + "." + std::to_string(_level);
	else strLable = std::to_string(_level);
	if (curNode->isLeaf)
	{
		IBPlusLNode* lcurNode = (IBPlusLNode*)curNode;
		std::cout << "Leaf level\n";
		for (int i = 0; i< curNode->length; i++)
		{
			std::cout << strLable << "\t" << std::to_string(lcurNode->count[i]) << "\t [" << curNode->interval[i].start << ", " << curNode->interval[i].end << "]\t #: " << std::to_string(i) << "\n";
			numPrint += 1;
		}
	}
	else
	{
		std::cout << "Non-leaf level\n";
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		int i = 0;
		for (; i< curNode->length; i++)
		{
			result = printInfoIBPlusTreeRec(lcurNode->pointer[i], strLable, i);
			std::cout << strLable << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].end << "]\t Max: " << lcurNode->max[i] << "\n";
			numPrint += 1;

			if (result == Constants::ERROR1)
			{
				std::cout << "++++++++++++++++++++++++++++<ERROR CODE: 1>++++++++++++++++++++++++++++++++\n";
				return result;
			}
		}
		//print last child
		result = printInfoIBPlusTreeRec(lcurNode->pointer[i], strLable, i);
		if (result == Constants::ERROR1)
		{
			std::cout << "++++++++++++++++++++++++++++<ERROR CODE: 2>++++++++++++++++++++++++++++++++\n";
			return result;
		}
		std::cout << strLable << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].end << "]\t Max: " << lcurNode->max[i] << "\n";
	}
	result = Constants::SUCCESS;
	return result;
}


//This recursive function is to print and save a nodes data and its childrens data in the IB+-Tree to the disk in addition to the console.
int IBPlusTree::printAllIBPlusTreeRec(IIBPlusNode* curNode, std::string _strLable, int _level)
{
	//print All data and info in IB+-Tree recursively
	int result = Constants::ERROR1;
	if (curNode == 0)
	{
		std::cout << "++++++++++++++++++++++++++++<ERROR CODE: 0>++++++++++++++++++++++++++++++++\n";
		return result;
	}
	std::string strLable = "";
	if (_strLable != "")
	{
		strLable = _strLable + "." + std::to_string(_level);
	}
	else
	{
		strLable = std::to_string(_level);
	}
	if (curNode->isLeaf)
	{
		IBPlusLNode* lcurNode = (IBPlusLNode*)curNode;
		std::cout << "Leaf level\n";
		foutput2 << "Leaf level\n";
		for (int i = 0; i< curNode->length; i++)
		{
			std::cout << strLable << "\t" << std::to_string(lcurNode->count[i]) << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].start << "]\t #: " << std::to_string(i) << "\n";
			foutput2 << strLable << "\t" << std::to_string(lcurNode->count[i]) << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].start << "]\t #: " << std::to_string(i) << "\n";
			for (int j = 0; j< lcurNode->count[i]; j++)
			{
				// Print data
				std::cout << "Data: " << std::string(lcurNode->data[i]->tuples[j]->value) << "\n";
				foutput2 << "Data: " << lcurNode->data[i]->tuples[j]->value << "\n";
			}
			numPrint += 1;
		}
	}
	else
	{
		std::cout << "Non-leaf level\n";
		foutput2 << "Non-leaf level\n";
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		int i = 0;
		for (; i< lcurNode->length; i++)
		{
			result = printAllIBPlusTreeRec(lcurNode->pointer[i], strLable, i);
			std::cout << strLable << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].end << "]\t Max: " << lcurNode->max[i] << "\n";
			foutput2 << strLable << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].end << "]\t Max: " << lcurNode->max[i] << "\n";
			numPrint += 1;

			if (result == Constants::ERROR1)
			{
				std::cout << "++++++++++++++++++++++++++++<ERROR CODE: 1>++++++++++++++++++++++++++++++++\n";
				return result;
			}
		}
		//print last child
		result = printAllIBPlusTreeRec(lcurNode->pointer[i], strLable, i);
		std::cout << strLable << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].end << "]\t Max: " << lcurNode->max[i] << "\n";
		foutput2 << strLable << "\t [" << lcurNode->interval[i].start << ", " << lcurNode->interval[i].end << "]\t Max: " << lcurNode->max[i] << "\n";

		if (result == Constants::ERROR1)
		{
			std::cout << "++++++++++++++++++++++++++++<ERROR CODE: 2>++++++++++++++++++++++++++++++++\n";
			return result;
		}
	}
	result = Constants::SUCCESS;
	return result;
}


//This function saves the current structure of the system as a metadata on the disk.
void IBPlusTree::save()
{
	writeMetaData();
	iBTree->writeMetaData();
}

//This function is used to save th IB+-Trees structure to the disk as a metadata before the application closes.
void IBPlusTree::writeMetaData()
{
	std::string tab = "";
	std::string	strWrite = tab;
	strWrite += std::to_string(rootNode->isLeaf) + "\n";
	strWrite += tab;
	strWrite += std::to_string(rootNode->level) + "," + std::to_string(rootNode->length) + "\n";
	strWrite += tab;
	int i = 0;
	for (; i < rootNode->length - 1; i++)
	{
		strWrite += rootNode->interval[i].start.toString() + "-" + rootNode->interval[i].end.toString() + "&";
	}
	strWrite += rootNode->interval[i].start.toString() + "-" + rootNode->interval[i].end.toString() + "\n";
	strWrite += tab;
	if (rootNode->isLeaf)
	{
		IBPlusLNode* lcurNode = (IBPlusLNode*)rootNode;
		for (int i = 0; i < rootNode->length; i++)
		{
			strWrite += std::to_string(lcurNode->count[i]);
			strWrite += "\n";
			std::string tab2 = tab + "\t";
			for (int j = 0; j < lcurNode->count[i]; j++)
			{
				strWrite += tab2;
				strWrite += lcurNode->data[i]->tuples[j]->key.toString() + "#" + lcurNode->data[i]->tuples[j]->value;
				strWrite += "\n";
			}
		}
	}
	else
	{
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)rootNode;
		int i = 0;
		for (; i < lcurNode->length; i++)
		{
			strWrite += lcurNode->max[i].toString() + "&";
		}
		strWrite += lcurNode->max[i].toString() + "\n";
	}

	std::ofstream* fout = new std::ofstream(Database + "/" + ibPlusMetaData, std::ofstream::out);
	(*fout) << strWrite;
	fout->close();
	if (!rootNode->isLeaf)
	{
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)rootNode;
		for (int i = 0; i < lcurNode->length + 1; i++)
		{
			writeMetaDataRec(lcurNode->pointer[i], tab + "\t");
		}
	}
}


//This is a recursive call to save each nodes structure as a metadata  on to the disk.
void IBPlusTree::writeMetaDataRec(IIBPlusNode* curNode, std::string tab)
{
	std::string	strWrite=tab;
	strWrite+=std::to_string(curNode->isLeaf)+"\n";
	strWrite+=tab;
	strWrite+= std::to_string(curNode->level) + "," + std::to_string(curNode->length)+"\n";
	strWrite+=tab;
	int i = 0;
	for (; i < curNode->length-1; i++)
	{
		strWrite +=curNode->interval[i].start.toString() + "-" + curNode->interval[i].end.toString() + "&";
	}
	strWrite +=curNode->interval[i].start.toString() + "-" + curNode->interval[i].end.toString() + "\n";
	strWrite+=tab;
	if (curNode->isLeaf)
	{
		IBPlusLNode* lcurNode = (IBPlusLNode*)curNode;
		for (int i = 0; i < curNode->length; i++)
		{
			strWrite += std::to_string(lcurNode->count[i]);
			strWrite += "\n";
			std::string tab2 = tab + "\t";
			for (int j = 0; j < lcurNode->count[i]; j++)
			{
				strWrite += tab2;
				strWrite += lcurNode->data[i]->tuples[j]->key.toString() + "#" + lcurNode->data[i]->tuples[j]->value;
				strWrite += "\n";
			}
		}
	}
	else
	{
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		int i = 0;
		for (; i < lcurNode->length; i++)
		{
			strWrite +=lcurNode->max[i].toString() + "&";
		}
		strWrite +=lcurNode->max[i].toString() + "\n";
	}

	std::ofstream* fout = new std::ofstream(Database+"/"+ibPlusMetaData, std::ofstream::out | std::ofstream::app);
	(*fout) << strWrite;
	fout->close();
	if (!curNode->isLeaf)
	{
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)curNode;
		for (int i = 0; i < lcurNode->length+1; i++)
		{
			writeMetaDataRec(lcurNode->pointer[i], tab+"\t");
		}
	}
}


//This function is used to restore the systems structure from the metadata which is written on the disk.
void IBPlusTree::read()
{
	readMetaData();
	iBTree->readMetaData();
}


//This function is used to restore the IB+-Trees structure from the metadata which is written on the disk.
int IBPlusTree::readMetaData()
{
	//Read and rebuild IB + -Tree
	int result = Constants::ERROR1;
	if (rootNode != 0)
	{
		//This block is  ran when the IB+-Tree already has its own structure.
		//So we have to ask the user to make sure if its previous structure should be removed or not. 
		std::cout << "IB+-Tree is not Null! All data in the IB+-Tree will be lost! Do you want to continue (y/n)?:";
		std::string str = "";
		std::getline(std::cin, str);
		if (str != "y") return result;
	}
	result = Constants::SUCCESS;
	std::string STRING;
	std::ifstream infile;
	infile.open(Database + "/" + ibPlusMetaData);
	bool b;
	getline(infile, STRING);
	b = (STRING == "1");
	rootNode = newIBPlusNode(b);
	getline(infile, STRING);
	std::vector<std::string> values = StringFunctions::Split(STRING, ',');
	rootNode->level = std::stoi(values[0]);
	rootNode->length = std::stoi(values[1]);
	getline(infile, STRING);
	values = StringFunctions::Split(STRING, '&');
	for (int i = 0; i < rootNode->length; i++)
	{
		std::vector<std::string> values2 = StringFunctions::Split(values[i], '-');
		rootNode->interval[i].start= std::stof(values2[0]);
		rootNode->interval[i].end = std::stof(values2[1]);
	}
	if (b)
	{
		IBPlusLNode* lcurNode = (IBPlusLNode*)rootNode;
		for (int i = 0; i < rootNode->length; i++)
		{
			getline(infile, STRING);
			lcurNode->count[i] = std::stoi(STRING);
			lcurNode->data[i] = new Bucket();
			for (int j = 0; j < lcurNode->count[i]; j++)
			{
				getline(infile, STRING);
				std::vector<std::string> values2 = StringFunctions::Split(STRING, '#');
				float key = std::stof(values2[0]);
				lcurNode->data[i]->tuples[j] = new Tuple(key,(char*) values2[1].c_str());
			}
		}
	}
	else
	{
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)rootNode;
		getline(infile, STRING);
		values = StringFunctions::Split(STRING, '&');
		for (int i = 0; i < rootNode->length+1; i++)
		{
			lcurNode->max[i]= std::stof(values[i]);
			IIBPlusNode* child=0;
			readMetaDataR(&infile, &child);
			lcurNode->pointer[i] = child;
		}
	}
	infile.close();
	return result;
}

//This is to restore the nodes of the IB+-Tree when the application opens.
int IBPlusTree::readMetaDataR(std::ifstream* infile, IIBPlusNode** node)
{
	int result = Constants::SUCCESS;
	std::string STRING;

	bool b;
	getline(*infile, STRING);
	b = (std::stoi(STRING) != 0);
	*node = newIBPlusNode(b);
	getline(*infile, STRING);
	std::vector<std::string> values = StringFunctions::Split(STRING, ',');
	(*node)->level = std::stoi(values[0]);
	(*node)->length = std::stoi(values[1]);
	getline(*infile, STRING);
	values = StringFunctions::Split(STRING, '&');
	for (int i = 0; i < (*node)->length; i++)
	{
		std::vector<std::string> values2 = StringFunctions::Split(values[i], '-');
		(*node)->interval[i].start = std::stof(values2[0]);
		(*node)->interval[i].end = std::stof(values2[1]);
	}
	if (b)
	{
		IBPlusLNode* lcurNode = (IBPlusLNode*)*node;
		for (int i = 0; i < (*node)->length; i++)
		{
			getline(*infile, STRING);
			lcurNode->count[i] = std::stoi(STRING);
			lcurNode->data[i] = new Bucket();
			for (int j = 0; j < lcurNode->count[i]; j++)
			{
				getline(*infile, STRING);
				std::vector<std::string> values2 = StringFunctions::Split(STRING, '#');
				float key = std::stof(values2[0]);
				lcurNode->data[i]->tuples[j] = new Tuple(key, (char*)values2[1].c_str());
			}
		}
	}
	else
	{
		IBPlusNLNode* lcurNode = (IBPlusNLNode*)*node;
		getline(*infile, STRING);
		values = StringFunctions::Split(STRING, '&');
		for (int i = 0; i < (*node)->length + 1; i++)
		{
			lcurNode->max[i] = std::stof(values[i]);
			IIBPlusNode* child=0;
			readMetaDataR(infile, &child);
			lcurNode->pointer[i] = child;
		}
	}
	return result;
}


//This function is used to create a tuple out of a key and value and change the settings regarding the memory.
Tuple* IBPlusTree::createTuple(Data key, char* Value)
{
	if (status.usedDA >= status.dataArea * 1024 * 2048) //self.dataArea * 1024 * 1024
	{
		//This is when the system detect =s the consumed memory has reached its limits.
		//so it will return a null object to force the system to group the data.
		printf("No more space");
		return 0;
	}
	Tuple* row = new Tuple(key, Value);
	int size = sizeof(key) + row->length;
	status.usedDA += size;
	status.availableDA -= size;
	return row;
}


//This function is used to deallocate buckets from the memory and change the settings regarding it.
int IBPlusTree::releaseBucket(Bucket* Bucket)
{
	int result = Constants::SUCCESS;
	int size = sizeof(float)*Constants::NUM_ROW_PER_BUCKET + Bucket->length;
	status.usedDA -= size;
	status.availableDA += size;
	if (status.usedDA < 0)
	{
		std::cout << "Error code 4: used space < 0! " + std::to_string(status.usedDA);
		result = Constants::ERROR1;
	}
	return result;
}
