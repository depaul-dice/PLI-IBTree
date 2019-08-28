#include "QueryManager.h"



//This function creates a new instance of the query manager
QueryManager::QueryManager(std::string dnName, IBPlusTree * IBPlusTree)
{
	iBPlusTree = IBPlusTree;
	fileCount = 0;

	std::thread([=] {
		loop();
	}).detach();
}



void QueryManager::loop()
{
	while (true)
	{
		lockState.lock();
		isQuery = false;
		isInsert = false;
		lockState.unlock();

		ConditionIsFinished.notify_all();


		lockQueue.lock();
		if (myqueue2.empty())
		{
			lockQueue.unlock();
			std::unique_lock<std::mutex> lck(timerMutex);

			loopNotifier.wait(lck);
			lockQueue.lock();
		}
		lockQueue.unlock();

		lockState.lock();
		isInsert = true;
		lockState.unlock();


		std::this_thread::sleep_for(std::chrono::milliseconds(Constants::MiliSecondsToWait));


		lockState.lock();
		isQuery = true;
		lockState.unlock();

		lockQueue.lock();
		int coun = myqueue2.size();
		ThreadInfo** p = new ThreadInfo*[coun];
		int i = 0;
		//std::cout << "count: " << coun << "\n\n";
		while (myqueue2.size() > 0)
		{

			p[i] = (myqueue2.front());
			myqueue2.pop();
			i++;
		}

		std::thread t1([=](ThreadInfo** pp, int count)
		{
			std::cout << "threads: " << count << "\n\n";
			ListBuckets* MergedBuckets = new ListBuckets();
			std::set<int> fileNumbers;
			for (int i = 0; i < count; i++)
			{
				pp[i]->b = new ListBuckets();
				iBPlusTree->iBTree->search(pp[i]->b, pp[i]->query);

				if (pp[i]->b->results.size() > 0)
				{
					std::set<BucketTree*, APtrComp>::iterator it = pp[i]->b->results.begin();
					for (; it != pp[i]->b->results.end(); ++it)
					{
						pp[i]->fileNumbers.insert((*it)->bucketID.fNum);
						fileNumbers.insert((*it)->bucketID.fNum);
						BucketTree* t = new BucketTree((*it)->bucketID, (*it)->interval);
						std::pair<std::set<BucketTree*>::iterator, bool> result = MergedBuckets->results.insert((t));
						if (!result.second) delete t;
					}

					*(pp[i]->fileAccess) = pp[i]->fileNumbers.size();
				}
				delete pp[i]->b;
			}

			for (int w = 0; w < count; w++)
			{
				*(pp[w]->totalFilesEach) = fileNumbers.size();
				if (*(pp[w]->fileAccess) == 0)
				{
					pp[w]->Finished.notify_one();
				}
			}

			if (MergedBuckets->results.size() > 0)
			{
				searchScanDisk(MergedBuckets, pp, count);
				iBPlusTree->threadManager->WaitForThreads();
				delete MergedBuckets;
			}
			for (int y = 0; y < count; y++)
			{
				pp[y]->b = 0;
				pp[y]->OutputTuples = 0;
				pp[y]->fileAccess = 0;
				pp[y]->totalFilesEach = 0;
				delete pp[y];
			}

		}, p, coun);
		t1.join();
		lockQueue.unlock();
	}
	return;
}



//This function goes through the bucket that has to be read and divides them into groups based on their file number and then be delivered to threads to be scanned.
//The first parameter is the sorted list if buckets(They get sorted according to their file number and offset).
//The second parameter is list of objects that each contains some information about the threads that send their query to the system.
//The third parameter is the number of threads that have sent their query. This would be equal to the length of the list(second parameter).
void QueryManager::searchScanDisk(ListBuckets* _outputBucket, ThreadInfo** pai, int count)
{
	if (_outputBucket->results.size() > 0)
	{
		std::mutex mtx4;
		int  fnum = 0;
		BufferInterface* buf = 0;

		std::set<BucketTree*, APtrComp>::iterator it = _outputBucket->results.begin();
		std::set<BucketTree*, APtrComp>::iterator ft = it;
		if ((*it)->bucketID.fNum != iBPlusTree->iBTree->bufferManager->mainWrittingBuffer->fNumber)
		{
			if (iBPlusTree->iBTree->bufferManager->writtingBuffer == 0 || (*it)->bucketID.fNum != iBPlusTree->iBTree->bufferManager->writtingBuffer->fNumber)
			{
				buf = new BufferInterface(iBPlusTree->iBTree->bufferManager->dataBase, (*it)->bucketID.fNum);
			}
			else
			{
				iBPlusTree->iBTree->bufferManager->writtingBuffer->isBeingRead = true;
				buf = iBPlusTree->iBTree->bufferManager->writtingBuffer;
			}
		}
		else
		{
			buf = iBPlusTree->iBTree->bufferManager->mainWrittingBuffer;
		}
		fnum = (*it)->bucketID.fNum;
		buf->readBucks.push_back(((BucketTree)**it));
		for (; it != _outputBucket->results.end(); ++it)
		{
			if ((*it)->bucketID.offset > -1)
			{
				if (fnum == (*it)->bucketID.fNum)
				{
					int in = buf->readBucks.size() - 1;
					if ((*it)->bucketID.offset == buf->readBucks[in].bucketID.offset + buf->readBucks[in].bucketID.size)
					{
						buf->readBucks[in].bucketID.size += (*it)->bucketID.size;
					}
					else
						buf->readBucks.push_back(((BucketTree)**it));
				}
				else
				{
					ft = it;

					std::mutex mtx4;
					if (fileCount>Constants::MaxConcurrentFiles)
					{
						std::unique_lock<std::mutex> lck(mtx4);
						fileAccessVariance.wait(lck);
					}

					fileCount++;
					iBPlusTree->threadManager->ReadScanFile(buf, pai, count, &fileCount/*, &fileCountMutex*/, &fileAccessVariance);

					if ((*it)->bucketID.fNum != iBPlusTree->iBTree->bufferManager->mainWrittingBuffer->fNumber)
					{
						if (iBPlusTree->iBTree->bufferManager->writtingBuffer != 0 && (*it)->bucketID.fNum == iBPlusTree->iBTree->bufferManager->writtingBuffer->fNumber)
						{
							bool deallocate = true;
							iBPlusTree->iBTree->bufferManager->writtingBuffer->isBeingRead = true;
							buf = iBPlusTree->iBTree->bufferManager->writtingBuffer;
						}
						else
						{
							bool deallocate = true;
							buf = new BufferInterface(iBPlusTree->iBTree->bufferManager->dataBase, (*it)->bucketID.fNum);
						}
					}
					else
					{
						buf = iBPlusTree->iBTree->bufferManager->mainWrittingBuffer;
					}

					fnum = (*it)->bucketID.fNum;
					buf->readBucks.push_back(((BucketTree)**it));
				}
			}
		}

		if (ft != it)
		{
			fileCount++;
			iBPlusTree->threadManager->ReadScanFile(buf, pai, count, &fileCount/*, &fileCountMutex*/, &fileAccessVariance);
		}
	}
}



int QueryManager::submit(Interval query, ReturningList* OutputTuples, int* FileAccess, int* Ft)
{
	lockState.lock();
	if(isQuery)
	{
		lockState.unlock();
		std::unique_lock<std::mutex> lck(lockConditionIsFinished);
		ConditionIsFinished.wait(lck);
		lockState.lock();
	}
	lockState.unlock();

	ThreadInfo* pv=new ThreadInfo();
	pv->query.start = query.start;
	pv->query.end = query.end;
	pv->OutputTuples = OutputTuples;
	pv->fileAccess=FileAccess;
	pv->totalFilesEach=Ft;


	lockQueue.lock();
	myqueue2.push(pv);
	loopNotifier.notify_one();
	lockQueue.unlock();


	std::mutex m2q2;
	{
	std::unique_lock<std::mutex> lck(m2q2);
	pv->Finished.wait(lck);
	}

	return 1;
}

int QueryManager::insertTuple(float key, char* Value)
{
	lockState.lock();
	if(isInsert)
	{
		lockState.unlock();
		std::unique_lock<std::mutex> lck(lockConditionIsFinished);
		ConditionIsFinished.wait(lck);
		lockState.lock();
		isQuery=true;
	}
	lockState.unlock();

	lockState.lock();
	isInsert=true;
	lockState.unlock();
	int result = 0;
	result= iBPlusTree->insertTuple(key,Value);
	iBPlusTree->threadManager->WaitForThreads();

	lockState.lock();
	isQuery=false;
	isInsert=false;
	lockState.unlock();
	ConditionIsFinished.notify_all();

	return result;
}







