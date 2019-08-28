#include "Interval.h"


//This function initializes a new intance of an interval.
Interval::Interval()
{
	start = 0.0;
	end = 0.0;
}

//This function initializes a new intance of an interval with a start(first parameter) and an end(second parameter) value.
Interval::Interval(Data Start, Data End)
{
	start = Start;
	end = End;
}