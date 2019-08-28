#ifndef generalTools_h
#define generalTools_h


#include "../PLITypes/Constants.h"
#include "../IBPlusTree/Types/IBPlusNode.h"
#include <math.h>
#include "Operations.h"
#include "../PLITypes/Data.h"



int findGoodPlace(IIBPlusNode * node,Data key, bool isLeaf);

#endif
