#include "RecordMax.h"


//This function initializes a new recordMax object.
recordMax::recordMax()
{
	leftMax = 0.0;
	rightMax = 0.0;
}

//This function initializes a new recordMax object with the given parameters.
recordMax::recordMax(float CurMax, float NewMax)
{
	leftMax = CurMax;
	rightMax = NewMax;
}