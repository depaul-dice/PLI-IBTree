#include "IBTree.h"


	//This function instantiates the IB-Tree of the system.
	IBTree::IBTree(std::string dnName, ThreadManager* threadManager)
	{
		rootNode = new IBNode();
		evaluation = Evaluation();
		Database = dnName;
		bufferManager = new BufferManager(dnName, threadManager);
	}


	//This function inserts a bucket into the IB-Tree. It will do the necessary chnages regarding the max value of the IB-Tree if it needs to.
	int IBTree::insertBucket(Bucket* _bucket)
	{
		Interval _interval = bufferManager->AddWritingBucket(_bucket);
		recordMax* recMax =new recordMax();
		recordRec* recRec = new recordRec();

		int result = Constants::ERROR1;
		if (!rootNode) return result;
		evaluation.setDBInterval(_interval);
		evaluation.countTotalBuckets();
		result = insertBucketRec(rootNode, _interval, _bucket->bucketID, recRec, recMax);
		if (result == Constants::OVERFLOW2)
		{
			//This is when an overflow occurs on the root which makes the root split.
			IBNode* newRoot = new IBNode();
			newRoot->length = 1;
			newRoot->level = rootNode->level + 1;
			newRoot->isLeaf = false;
			newRoot->interval[0]= recRec->selEntry->interval;
			newRoot->bucketID[0] = recRec->selEntry->bucketID;


			if (newRoot->interval[0].end < recMax->rightMax)
			{
				//This is when the bucket which is going to be set as the roots one and only bucket does not have a tuple with the key, bigger that the maximum of right hand side's maximum.
				newRoot->max[0] = recMax->rightMax;
			}
			else
			{
				//This is when the bucket which is going to be set as the roots one and only bucket has a tuple with the key, bigger that the maximum of right hand side's maximum.
				newRoot->max[0] = newRoot->interval[0].end;
			}

			newRoot->max[1] = recMax->leftMax;
			newRoot->pointer[0] = rootNode;
			newRoot->pointer[1] = recRec->newNode;
			rootNode = newRoot;
			result = Constants::SUCCESS;
		}

		return result;
	}


	//This function inserts a bucket into the IB-Tree. It will do the necessary chnages regarding the max value of the IB-Tree if it needs to.
	//The _interval parameter is the range or the boundry of the tuples which the bucket contains.
	//The _bucketID parameter contains informations about the bucket(file number of the files that is written to it, the offset of were it is written, ...)
	int IBTree::insertBucket(Interval _interval, BucketInfo _bucketID)
	{
		recordMax* recMax = new recordMax();
		recordRec* recRec = new recordRec();

		int result = Constants::ERROR1;
		if (!rootNode) return result;
		evaluation.setDBInterval(_interval);
		evaluation.countTotalBuckets();
		result = insertBucketRec(rootNode, _interval, _bucketID, recRec, recMax);
		if (result == Constants::OVERFLOW2)
		{
			//This is when an overflow occurs on the root which makes the root split.
			IBNode* newRoot = new IBNode();
			newRoot->length = 1;
			newRoot->level = rootNode->level + 1;
			newRoot->isLeaf = false;
			newRoot->interval[0]= recRec->selEntry->interval;
			newRoot->bucketID[0] = recRec->selEntry->bucketID;


			if (newRoot->interval[0].end < recMax->rightMax)
			{
				//This is when the bucket which is going to be set as the roots one and only bucket does not have a tuple with the key, bigger that the maximum of right hand side's maximum.
				newRoot->max[0] = recMax->rightMax;
			}
			else
			{
				//This is when the bucket which is going to be set as the roots one and only bucket has a tuple with the key, bigger that the maximum of right hand side's maximum.
				newRoot->max[0] = newRoot->interval[0].end;
			}

			newRoot->max[1] = recMax->leftMax;
			newRoot->pointer[0] = rootNode;
			newRoot->pointer[1] = recRec->newNode;
			rootNode = newRoot;
			result = Constants::SUCCESS;
		}

		return result;
	}


	//This function inserts a bucket into the IB-Tree.
	//curNode is the node which will be verified whether the bucket has to be inserted in it.
	//_interval is the range of the tuples which the buckets that needs to be inserted contains.
	//The _bucketID parameter contains informations about the bucket(file number of the files that is written to it, the offset of were it is written, ...)
	//recRec is an structure that in case of a split, contains information about the second part of the node that has been splitted and needs to be inserted into its parents nodes.
	//recmax stores the left hand side and the right hand side maximum of the moving node.
	int IBTree::insertBucketRec(IBNode* curNode, Interval _interval, BucketInfo _bucketID, recordRec* recRec, recordMax* recMax)
	{
		int result = Constants::ERROR1;
		if (!curNode) return result;
		if (curNode->isLeaf) //LEAF - NODE
		{
			//Finds a place to insert
			if (curNode->length >= Constants::MAX_NUM_ENTRY)
			{
				//This is when we reach to a leaf which does not have any space so it will and has to return overflow.
				recRec->newNode = new IBNode();
				recMax->leftMax = Constants::MIN_DISTANCE;
				recMax->rightMax = Constants::MIN_DISTANCE;
				//Find a place to split
				int goodPlace = curNode->length;
				for (int i = 0; i < curNode->length; i++)
				{
					if (curNode->interval[i].start > _interval.start)
					{
						goodPlace = i;
						break;
					}
				}
				//#1. Move data to buffer
				IBNodeBuffer* buffer = new IBNodeBuffer();
				copyNodeToBuffer(buffer, curNode, _bucketID, _interval, -1.0, -1.0, 0, goodPlace, Constants::MAX_NUM_ENTRY);
				//#2. Copy from buffer
				copyNodeFromBuffer(buffer, curNode, 0, Constants::MAX_NUM_ENTRY / 2 - 1);
				copyEntryFromBuffer(buffer, recRec->selEntry, Constants::MAX_NUM_ENTRY / 2);
				copyNodeFromBuffer(buffer, recRec->newNode, Constants::MAX_NUM_ENTRY / 2 + 1, Constants::MAX_NUM_ENTRY);
				//#3. Finialize
				recRec->newNode->isLeaf = true;
				recRec->newNode->level = 0;
				recRec->newNode->length = Constants::MAX_NUM_ENTRY / 2;
				//Finds the left hand side maximum.
				for (int i = 0; i < Constants::MAX_NUM_ENTRY / 2; i++)
				{
					if (recMax->leftMax < recRec->newNode->max[i]) recMax->leftMax = recRec->newNode->max[i];
				}
				curNode->length = Constants::MAX_NUM_ENTRY / 2;
				//Finds the right hand side maximum.
				for (int i = 0; i < Constants::MAX_NUM_ENTRY / 2; i++)
				{
					if (recMax->rightMax < curNode->max[i]) recMax->rightMax = curNode->max[i];
				}
				result = Constants::OVERFLOW2; //return overflow;
				delete buffer;
			}
			else
			{
				//This is when the leaf is not full yet.
				int goodPlace = curNode->length;
				//Check if the current max value is the highest value
				bool isHighest = true;
				for (int i = 0; i < curNode->length + 1; i++)
				{
					if (_interval.end < curNode->max[i])
					{
						isHighest = false;
						break;
					}
				}


				//Find a place to insert
				for (int i = 0; i < curNode->length; i++)
				{
					if (curNode->interval[i].start > _interval.start)
					{
						goodPlace = i;
						break;
					}
				}

				//Moving the node that the bucket wants to be inserted in it to buffer and add the bucket in the appropriate space,
				IBNodeBuffer* buffer = new IBNodeBuffer();
				copyNodeToBuffer(buffer, curNode, _bucketID, _interval, -1.0, -1.0, 0, goodPlace, Constants::MAX_NUM_ENTRY);

				//Copy from buffer to the previous node.
				copyNodeFromBuffer(buffer, curNode, 0, curNode->length);
				//3. Finialize
				curNode->length += 1;

				if (isHighest) result = Constants::SUCCESS_UPDATE_MAX;
				else result = Constants::SUCCESS;
				delete buffer;
			}
		}
		else  //#NON - LEAF NODE
		{
			//This finds the appropriate child in the node to go down the tree
			int goodPlace = curNode->length;
			for (int i = 0; i < curNode->length; i++)
			{
				if (curNode->interval[i].start > _interval.start)
				{
					goodPlace = i;
					break;
				}
			}

			//Recursive call to the insert function
			result = insertBucketRec(curNode->pointer[goodPlace], _interval, _bucketID, recRec, recMax);

			if (result == Constants::SUCCESS_UPDATE_MAX)
			{
				// Update max value for the node at the current possition
				if (goodPlace < curNode->length)
				{
					if (curNode->interval[goodPlace].end < _interval.end) curNode->max[goodPlace] = _interval.end;
				}
				else curNode->max[goodPlace] = _interval.end;
				// Check if the current max value is the highest value
				result = Constants::SUCCESS;
				for (int i = 0; i < curNode->length + 1; i++)
				{
					if (_interval.end >= curNode->max[i])
					{
						curNode->max[i] = _interval.end;
						result = Constants::SUCCESS_UPDATE_MAX; //Propagate to higher level
						break;
					}
				}
			}
			else
			{
				if (result == Constants::OVERFLOW2)
				{
					//This is when an overflow  had occured on the child node and its middle bucket needs to be moved to the parent(the current node).
					if (curNode->length >= Constants::MAX_NUM_ENTRY)
					{
						//The parent (the current node) does not has any space to fit the bucket (childs middle bucket).
						//So it should return an oveflow.
						IBEntry* tempEntry = new IBEntry();
						IBNode* tempNode = new IBNode();
						//1. Move to buffer
						IBNodeBuffer* buffer = new IBNodeBuffer();
						copyNodeToBuffer(buffer, curNode, recRec->selEntry->bucketID, recRec->selEntry->interval, recMax->rightMax, recMax->leftMax, recRec->newNode, goodPlace, Constants::MAX_NUM_ENTRY);
						//2. Copy from buffer
						copyNodeFromBuffer(buffer, curNode, 0, Constants::MAX_NUM_ENTRY / 2 - 1);
						copyEntryFromBuffer(buffer, tempEntry, Constants::MAX_NUM_ENTRY / 2);
						copyNodeFromBuffer(buffer, tempNode, Constants::MAX_NUM_ENTRY / 2 + 1, Constants::MAX_NUM_ENTRY);
						//3. finalize
						tempNode->isLeaf = curNode->isLeaf;
						tempNode->level = curNode->level;
						tempNode->length = Constants::MAX_NUM_ENTRY / 2;
						curNode->length = Constants::MAX_NUM_ENTRY / 2;
						for (int i = 0; i < Constants::MAX_NUM_ENTRY / 2 + 1; i++)
						{
							if (recMax->leftMax < tempNode->max[i]) recMax->leftMax = tempNode->max[i];
						}
						if (recMax->leftMax < tempEntry->interval.end) recMax->leftMax = tempEntry->interval.end;
						for (int i = 0; i < Constants::MAX_NUM_ENTRY / 2 + 1; i++)
						{
							if (recMax->rightMax < curNode->max[i]) recMax->rightMax = curNode->max[i];
						}
						recRec->selEntry = tempEntry;
						recRec->newNode = tempNode;
						result = Constants::OVERFLOW2;
						delete buffer;
					}
					else //curNode.length < Constants.MAX_NUM_ENTRY
					{
						// The parent(the current node) has space to fit the bucket(childs middle bucket).
						//1. Move to buffer
						IBNodeBuffer* buffer = new IBNodeBuffer();
						copyNodeToBuffer(buffer, curNode, recRec->selEntry->bucketID, recRec->selEntry->interval, recMax->rightMax, recMax->leftMax, recRec->newNode, goodPlace, curNode->length);
						//#2. Copy from buffer
						copyNodeFromBuffer(buffer, curNode, 0, curNode->length);
						//3. finalize
						curNode->length += 1;
						//return SUCCESS or SUCCESS_UPDATE_MAX
						result = Constants::SUCCESS;
						for (int i = 0; i < curNode->length + 1; i++)
						{
							if (_interval.end >= curNode->max[i])
							{
								result = Constants::SUCCESS_UPDATE_MAX;
								break;
							}
						}
						delete buffer;
					}
				}
			}
		}
		return result;
	}

	//This function is used to query the database.
	//The first parameter is a pointer to object that holds the found tuples.
	//The second parameter in the range which the search going to be based on.
	int IBTree::search(ListBuckets* _output, Interval _interval)
	{
		int result = Constants::ERROR1;
		if (!rootNode) return result;
		result = searchRec(rootNode, _output, _interval);
		evaluation.setInterval(_interval);
		return result;
	}


	//This function is used to find the nodes that intersect with the query.
	//The first parameter in the node which the query is operating on.
	//The second parameter is a pointer to object that holds the found tuples.
	//The third parameter in the range which the search going to be based on.
	int IBTree::searchRec(IBNode* curNode, ListBuckets* _output, Interval _interval)
	{
		int	result = Constants::SUCCESS;
		if (curNode==0)
		{
			//This is to check whether the current node points to a real object or not.
			std::cout<< "+++++++++++++++++++++++++++<ERROR CODE: 3>++++++++++++++++++++++++++++++++";
			result = Constants::ERROR1;
			return result;
		}
		if (curNode->isLeaf)
		{
			//This is when there is no more nodes in the IB-Tree to go down to
			// so we have to go through its buckets to find the ones that intersect with the query.
			for (int i = 0; i < curNode->length; i++)
			{
				evaluation.countSE();
				if (_interval.end < curNode->interval[i].start) break;
				if (intersect(_interval, curNode->interval[i]))
				{
					if (curNode->bucketID[i].offset > -1)
					{
						_output->results.insert(new BucketTree(curNode->bucketID[i], curNode->interval[i]));
					}
				}
			}
		}
		else
		{
			//This part finds all the children that we have to go down to and
			//also find the buckets in the current node which may contain tuples thatmay match the query.
			int index = 0;
			//All entries and children(except last child)
			for (int i = 0; i < curNode->length; i++)
			{
				if (result == Constants::ERROR1) return result;
				evaluation.countSE();
				index += 1;

				//Recursive call to go down the tree.
				if ((_interval.start <= curNode->max[i]) && (curNode->pointer[i]!=0)) result = searchRec(curNode->pointer[i], _output, _interval);
				if (_interval.end < curNode->interval[i].start) return result;

				//This is to examine the current bucket to see whether it should be added to the result or not.
				if (intersect(_interval, curNode->interval[i]))
				{
					if (curNode->bucketID[i].offset > -1)
					{
						_output->results.insert(new BucketTree(curNode->bucketID[i], curNode->interval[i]));
					}
				}
			}
			//last child
			if ((_interval.start <= curNode->max[index]) && (curNode->pointer[index]!=0)) result = searchRec(curNode->pointer[index], _output, _interval);
		}
		return result;
	}




	//Print
	int IBTree::printIBTree()
	{
		//print IB - Tree
		int result = Constants::ERROR1;
		if (rootNode==0) return result;
		numPrint = 0;
		result = printIBTreeRec(rootNode, "", 0);
		printf("=================================== Number of elements: %i ===========================================\n", numPrint);
		return result;
	}


	int IBTree::printIBTreeRec(IBNode* curNode, std::string _strLable, int _level)
	{
		//print IB-Tree recursively
		int result = Constants::ERROR1;
		if (!curNode)
		{
			printf("++++++++++++++++++++++++++++<ERROR CODE: 0>++++++++++++++++++++++++++++++++");
			return result;
		}
		std::string strLable;
		if (_strLable != "") strLable = _strLable + "." + std::to_string(_level);
		else
		{
			strLable = std::to_string(_level);
		}
		if (curNode->isLeaf)
		{
			printf( "Leaf level\n");
			for (int i = 0; i < curNode->length; i++)
			{
				printf( "%s\t%i\t [%f, %f]\t #: %i\n", strLable.c_str(), curNode->bucketID[i], curNode->interval[i].start, curNode->interval[i].end,i);
				numPrint += 1;
			}
		}
		else
		{
			printf("Non-leaf level\n");
			int i = 0;
			for (; i < curNode->length; i++)
			{
				numPrint += 1;
				result = printIBTreeRec(curNode->pointer[i], strLable, i);
				if (result == Constants::ERROR1)
				{
					printf("++++++++++++++++++++++++++++<ERROR CODE: 1>++++++++++++++++++++++++++++++++\n");
					return result;
				}
				printf("%s\t%i\t [%f, %f]\t Max: %f\n", strLable.c_str(), curNode->bucketID[i], curNode->interval[i].start, curNode->interval[i].end, curNode->max[i]);
			}
			result = printIBTreeRec(curNode->pointer[i], strLable, i);
			if (result == Constants::ERROR1)
			{
				printf("++++++++++++++++++++++++++++<ERROR CODE: 2>++++++++++++++++++++++++++++++++\n");
				return result;
			}
			printf("%s\t%i\t [%f, %f]\t Max: %f\n", strLable.c_str(), curNode->bucketID[i], curNode->interval[i].start, curNode->interval[i].end, curNode->max[i]);
		}
		result = Constants::SUCCESS;
		return result;
	}

	//This function is used to save th IB-Tree to the disk before the application closes.
	void IBTree::writeMetaData()
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
		std::string tab2 = tab + "\t";
		for (int i = 0; i < rootNode->length; i++)
		{
			strWrite += tab2;
			strWrite += std::to_string(rootNode->bucketID[i].fNum) + "%" + std::to_string(rootNode->bucketID[i].offset) + "%" + std::to_string(rootNode->bucketID[i].size) + "\n";
		}
		strWrite += tab;
		{
			int i = 0;
			for (; i < rootNode->length; i++)
			{
				strWrite += rootNode->max[i].toString() + "&";
			}
			strWrite += rootNode->max[i].toString() + "\n";
		}
		std::ofstream* fout = new std::ofstream(Database + "/" + ibTreeMetaData, std::ofstream::out);
		(*fout) << strWrite;
		fout->close();
		if (!rootNode->isLeaf)
		{
			for (int i = 0; i < rootNode->length + 1; i++)
			{
				writeMetaDataRec(rootNode->pointer[i], tab + "\t");
			}
		}
		bufferManager->save();
	}



	//This is a recursive call to save each nodes structure as a metadata  on to the disk.
	void IBTree::writeMetaDataRec(IBNode* curNode, std::string tab)
	{
		std::string	strWrite = tab;
		strWrite += std::to_string(curNode->isLeaf) + "\n";
		strWrite += tab;
		strWrite += std::to_string(curNode->level) + "," + std::to_string(curNode->length) + "\n";
		strWrite += tab;
		int i = 0;
		for (; i < curNode->length - 1; i++)
		{
			strWrite += curNode->interval[i].start.toString() + "-" + curNode->interval[i].end.toString() + "&";
		}
		strWrite += curNode->interval[i].start.toString() + "-" + curNode->interval[i].end.toString() + "\n";
			std::string tab2 = tab + "\t";
			for (int i = 0; i < curNode->length; i++)
			{
				strWrite += tab2;
				strWrite += std::to_string(curNode->bucketID[i].fNum) + "%"+ std::to_string(curNode->bucketID[i].offset) + "%"+ std::to_string(curNode->bucketID[i].size) + "\n";
			}
		strWrite += tab;
		{
			int i = 0;
			for (; i < curNode->length; i++)
			{
				strWrite += curNode->max[i].toString() + "&";
			}
			strWrite += curNode->max[i].toString() + "\n";
		}

		std::ofstream* fout = new std::ofstream(Database + "/" + ibTreeMetaData, std::ofstream::out | std::ofstream::app);
		(*fout) << strWrite;
		fout->close();
		if (!curNode->isLeaf)
		{
			for (int i = 0; i < curNode->length + 1; i++)
			{
				writeMetaDataRec(curNode->pointer[i], tab + "\t");
			}
		}
	}

	//This is to restore the IB-Tree when the application opens.
	int IBTree::readMetaData()
	{
		int result = Constants::SUCCESS;
		std::string STRING;
		std::ifstream infile;
		infile.open(Database + "/" + ibTreeMetaData, std::ofstream::in);
		bool b;
		getline(infile, STRING);
		b = (STRING == "1");
		rootNode->isLeaf = b;
		getline(infile, STRING);
		std::vector<std::string> values = StringFunctions::Split(STRING, ',');
		rootNode->level = std::stoi(values[0]);
		rootNode->length = std::stoi(values[1]);
		getline(infile, STRING);
		values = StringFunctions::Split(STRING, '&');
		for (int i = 0; i < rootNode->length; i++)
		{
			std::vector<std::string> values2 = StringFunctions::Split(values[i], '-');
			rootNode->interval[i].start = std::stof(values2[0]);
			rootNode->interval[i].end = std::stof(values2[1]);
		}
		for (int j = 0; j < rootNode->length; j++)
		{
			getline(infile, STRING);
			std::vector<std::string> values2 = StringFunctions::Split(STRING, '%');
			rootNode->bucketID[j].fNum = std::stoi(values2[0]);
			rootNode->bucketID[j].offset = std::stoi(values2[1]);
			rootNode->bucketID[j].size = std::stoi(values2[2]);
		}
			getline(infile, STRING);
			values = StringFunctions::Split(STRING, '&');
			for (int i = 0; i < rootNode->length + 1; i++)
			{
				rootNode->max[i] = std::stof(values[i]);
				if (!b)
				{
					IBNode* child = 0;
					readMetaDataRec(&infile, &child);
					rootNode->pointer[i] = child;
				}
			}
		infile.close();
		bufferManager->read();
		return result;
	}

	//This is to restore the nodes of the IB-Tree when the application opens.
	int IBTree::readMetaDataRec(std::ifstream* infile, IBNode** node)
	{
		int result = Constants::SUCCESS;
		std::string STRING;

		bool b;
		getline(*infile, STRING);
		b = (std::stoi(STRING) != 0);
		(*node) = new IBNode();
		(*node)->isLeaf = b;
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
		for (int j = 0; j < (*node)->length; j++)
		{
			getline(*infile, STRING);
			std::vector<std::string> values2 = StringFunctions::Split(STRING, '%');
			(*node)->bucketID[j].fNum = std::stoi(values2[0]);
			(*node)->bucketID[j].offset = std::stoi(values2[1]);
			(*node)->bucketID[j].size = std::stoi(values2[2]);
		}
			getline(*infile, STRING);
			values = StringFunctions::Split(STRING, '&');
			for (int i = 0; i < (*node)->length + 1; i++)
			{
				(*node)->max[i] = std::stof(values[i]);
				if (!b)
				{
					IBNode* child = 0;
					readMetaDataRec(infile, &child);
					(*node)->pointer[i] = child;
					//std::cout << child->level << "f\n";
					//t++;
				}
			}
		return result;
	}



	IBNode* IBTree::getRootNode()
	{
		return rootNode;
	}

	int IBTree::getSE()
	{
		return evaluation.getSE();
	}

	int IBTree::printEvalInfo()
	{
		evaluation.printEvalInfo();
		return Constants::SUCCESS;
	}

