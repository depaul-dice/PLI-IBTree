#include "Tuple.h"



//This function destructs a tuple object.
Tuple::~Tuple()
{
	free(value);
}


//This function initializes a new tuple with the given parameters.
Tuple::Tuple(Data Key, char* Value)
{
	key = Key;
	//value = strdup(Value);
	length = strlen(Value) + 1;
	value = (char *)malloc(length);
#ifdef _WIN64
	strcpy_s(value, length, Value);
#elif _WIN32 || __linux__
	strcpy(value, Value);
#endif
}
