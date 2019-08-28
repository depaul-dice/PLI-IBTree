#include "FloatingPoint.h"

#include <float.h>
#include <cfloat>
#include <math.h>
#include <algorithm>

namespace FloatingPoint
{
	//This function checks whether two floating are equal or not.
	bool almostEqual(float a, float b)
	{
		return fabs(a - b) <= FLT_EPSILON;
	}


	#define valid_digit(c) ((c) >= '0' && (c) <= '9')

	//This function converts a C-String to a float 
	float ToFloat(const char *p)
	{
		float r = 0.0;
		while (valid_digit(*p))
		{
			r = (r * 10) + (*p++ - '0');
		}

		if (*p == '.')
		{
			float f = 0.0;
			float scale = 1.0;
			++p;
			while (*p != '\0')
			{
				f = (f * 10) + (*p++ - '0');
				scale *= 10;
			}
			r += f / scale;
		}

		return r;
	}
}


