
#include "timeTools.h"
#include <ctime>
#include <cassert>
#include <chrono>

#include <cstdlib>

	//This function instantiates the timer object.
	timer::timer()
	{
		result = 0.0;
		resultInSecond = 0.0;
		resultInMilliSecond = 0.0;
		resultInMicroSecond = 0.0;
	}

	//This function starts the timer. 
	void timer::start()
	{
		startCounter = std::chrono::system_clock::now();
	}

	//This function stops the timer from running and records the time passed.
	void timer::end()
	{
		endCounter = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = endCounter - startCounter;
		result += elapsed.count();
		resultInSecond = result;
		resultInMilliSecond = result * 1000.0;
		resultInMicroSecond = result * 1000000.0;
	}

	//This function sets the timer to zero and restarts it again.
	void timer::reStart()
	{
		startCounter = std::chrono::system_clock::now();
		endCounter = startCounter;
		result = 0.0;
		resultInSecond = 0.0;
		resultInMilliSecond = 0.0;
		resultInMicroSecond = 0.0;
	}

	//This function is used to restarts the timer at a specific time.
	void timer::reStartValue(double value)
	{
		startCounter = std::chrono::system_clock::now();
		endCounter = startCounter;
		result = value;
		resultInSecond = value;
		resultInMilliSecond = value * 1000.0;
		resultInMicroSecond = value * 1000000.0;
	}


	double timer::getResult()
	{
		return result;
	}

	double timer::getResultInSecond()
	{
		return resultInSecond;
	}

	double timer::getResultInMilliSecond()
	{
		return resultInMilliSecond;
	}

	double timer::getResultInMicroSecond()
	{
		return resultInMicroSecond;
	}
