#ifndef Tuple_h
#define Tuple_h

#include <cstring>
#include <stdlib.h>
#include "../../Data.h"

class Tuple
{
public:
	Data key = 0.0;
	char* value;
	int length = 0;
	~Tuple();
	Tuple(Data Key, char* Value);
};


#endif
