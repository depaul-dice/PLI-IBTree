#ifndef timeTools_h
#define timeTools__h

#include <ctime>

//#include <pqxx/pqxx>

#include <cassert>
#include <chrono>

#include <cstdlib>

class timer
{
	/*clock_t startCounter = 0.0;
	clock_t endCounter = 0.0;
	double  result = 0.0;
	double  resultInSecond = 0.0;
	double  resultInMilliSecond = 0.0;
	double  resultInMicroSecond = 0.0;*/

	std::chrono::system_clock::time_point startCounter;
	std::chrono::system_clock::time_point endCounter;
	double  result = 0.0;
	double  resultInSecond = 0.0;
	double  resultInMilliSecond = 0.0;
	double  resultInMicroSecond = 0.0;

public:
	timer();

	void start();

	void end();

	void reStart();


	void reStartValue(double value);


	double getResult();

	double getResultInSecond();

	double getResultInMilliSecond();

	double getResultInMicroSecond();
};


#endif
