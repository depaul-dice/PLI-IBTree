#include "ReadingScanningThread.h"



ReadingScanningThread::ReadingScanningThread(BufferInterface* ReadingBuffer, ThreadInfo** Pai, int Count, std::mutex* OutputMutex, int* ReadingThreadCount/*, std::mutex* ThreadCountMutex*/, std::condition_variable* ThreadsVariance, int* FileCount/*, std::mutex* FileCountMutex*/, std::condition_variable* FileCountVariance, std::condition_variable* IsReadable)
{
	// TODO Auto-generated constructor stub
	pai = Pai;
	count = Count;
	rBuffer = ReadingBuffer;
	isReadable = IsReadable;

	_outputTuples2 = new ReturningList*[Count];
	for (int y = 0; y < count; y++)
	{
		_outputTuples2[y]= new ReturningList();
	}
	outputMutex = OutputMutex;


	fileCount = FileCount;
	fileCountVariance = FileCountVariance;
	//fileCountMutex = FileCountMutex;


	reaingThreadCount= ReadingThreadCount;
	//threadCountMutex= ThreadCountMutex;
	threadsVariance= ThreadsVariance;
}

ReadingScanningThread::~ReadingScanningThread()
{
	// TODO Auto-generated destructor stub
}








std::thread ReadingScanningThread::run()
{
	std::thread t1([this]() { Read(); });
	t1.detach();
	return t1;
}

int g = 0;
void ReadingScanningThread::Read()
{
	g++;
	//std::cout << rBuffer->fNumber << "\n";
	running = true;
	char* data = 0;

	FILE* pFile;
	std::string s = "./" + rBuffer->dataBase + "/" + rBuffer->dataBase + "-" + std::to_string(rBuffer->fNumber);

#ifdef _WIN64
	fopen_s(&pFile, s.c_str(), "r");
#elif _WIN32 || __linux__
	pFile = fopen(s.c_str(), "r");
#endif

	bool ncomplete = true;

	if (rBuffer->type == 0)
	{
		if (rBuffer->readBucks.size() > Constants::BucketLimit)
		{
			ncomplete = false;
			if (pFile)
			{
				fseek(pFile, 0, SEEK_END);   // non-portable
				int size = ftell(pFile);
				data = (char*)malloc(size);
				fseek(pFile, 0, SEEK_SET);
				fread(data, 1, size, pFile);
			}
		}
	}
	else
	{
		//ncomplete = false;
		std::mutex mtx4;
		if (rBuffer->isBeingWritten)
		{
			std::unique_lock<std::mutex> lck(mtx4);
			isReadable->wait(lck);
		}
		data = ((WritingBuffer*)rBuffer)->data;
	}

	//std::this_thread::sleep_for(std::chrono::milliseconds(40));

	int o = 0;int p = 0;
	std::vector<ThreadInfo*> ps;

	(outputMutex)->lock();
	for (int y = 0; y < count; y++)
	{
		if(pai[y]->fileNumbers.find(rBuffer->fNumber) != pai[y]->fileNumbers.end())
		{
			ps.push_back(pai[y]);
		}
	}
	(outputMutex)->unlock();

	for (int i = 0; i < rBuffer->readBucks.size(); i++)
	{
		char* rbuf = data;
		if (rBuffer->type == 0)
		{
			if (ncomplete)
			if (rBuffer->readBucks[i].bucketID.fNum == rBuffer->fNumber)
			{
				//std::cout<<"ndyjty\n";
				fseek(pFile, rBuffer->readBucks[i].bucketID.offset, SEEK_SET);
				data = (char*)malloc(rBuffer->readBucks[i].bucketID.size);
				fread(data, 1, rBuffer->readBucks[i].bucketID.size, pFile);
				rbuf = data;

				//std::this_thread::sleep_for(std::chrono::milliseconds(40));
			}
		}
		else
		{
			rbuf = data + rBuffer->readBucks[i].bucketID.offset;
		}

		size_t next = 0;
		size_t end = rBuffer->readBucks[i].bucketID.size;
		char* x=rbuf;
		{
			int l;
			while (next < end-1)
			{
				memcpy(&l, (char*)x+0, sizeof(int));
				x+=sizeof(int);
				char* buf2 = x;
				int z = 0;
				while (z < 12)
				{
					if (buf2[0] == ',')
					{
						z++;
					}
					buf2++;
				}
				char* buf3 = buf2;
				while (buf3[0] != ',') buf3++;
				int q = buf3 - buf2;
				char *f = (char*)malloc(q + 1);
				memcpy(f, buf2, q);
				f[q] = '\0';
				float s = FloatingPoint::ToFloat(f);
				//float s = std::atof(f);
				for (int y = 0; y < ps.size(); y++)
				{
					bool q = ps[y]->query.start <= s;
					bool w = s <= ps[y]->query.end;
					if (q && w)
					{
						char* buf1 = (char*)malloc(l);
						memcpy(buf1, x, l);
						_outputTuples2[y]->Add(buf1);
						o++;
						//std::cout<<buf1<<"\n";
					}
				}
				free(f);
				x+=l;
				next += l + sizeof(int);
			}
		}

		if (ncomplete)
		{
			if (rBuffer->type == 0) free(data);
		}
	}

	if (pFile)
	{
		fclose(pFile);
	}

	(outputMutex)->lock();

	for (int y = 0; y < ps.size(); y++)
	{
		if(_outputTuples2[y]->size>0)
		{
			ps[y]->outputMutex.lock();
			ps[y]->OutputTuples->Add(_outputTuples2[y]);
			ps[y]->outputMutex.unlock();
		}

		//if(rBuffer->fNumber==(*(ps[y]->b->results.rbegin()))->bucketID.fNum)
		{
			std::set<int>::iterator it;
			  it = ps[y]->fileNumbers.find (rBuffer->fNumber);
			  ps[y]->fileNumbers.erase (it);

			  if(ps[y]->fileNumbers.size()<1)
			  {
					//if(ps[y]->b->results.size()>0)
					{
						//delete ps[y]->b;
					}
			ps[y]->Finished.notify_one();
			  }
			//ps[y]->Finished.notify_all();
		}
	}

	(*reaingThreadCount)--;
	(*fileCount)--;
	(outputMutex)->unlock();
	(threadsVariance)->notify_one();
	(fileCountVariance)->notify_one();

	running = true;
	done = true;

	if (!ncomplete)
	{
		if (rBuffer->type == 0) free(data);
	}
}

