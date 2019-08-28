#ifndef ThreadInterface_h
#define ThreadInterface_h

#include <thread>
#include <mutex>
#include <condition_variable>


class ThreadInterface
{
public:
	bool running=false;
	bool done=false;
};



#endif
