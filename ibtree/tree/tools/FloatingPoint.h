#ifndef IBTREE_TREE_TOOLS_FLOATINGPOINT_H_
#define IBTREE_TREE_TOOLS_FLOATINGPOINT_H_

namespace FloatingPoint
{
extern "C" bool almostEqual(float a, float b);

extern "C" float ToFloat(const char *p);
}

#endif
