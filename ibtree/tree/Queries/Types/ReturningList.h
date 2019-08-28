#ifndef ReturningList_h
#define ReturningList_h

#include <malloc.h>

struct Record
{
	char* content;
	Record *next;

	Record(char* Content)
	{
		content = Content;
		next = 0;
	}
};

class ReturningList
{
public:
	ReturningList()
	{
		head = 0;
		size = 0;
	}

	~ReturningList()
	{
		Record *next = head;

		while (next)
		{
			Record *deleteMe = next;
			next = next->next;
			free(deleteMe->content);
			delete deleteMe;
		}
	}

	void Add(char* val)
	{
		Record *n = new Record(val);
		n->next = head;
		head = n;
		size++;
	}

	void Add(ReturningList* pOrder)
	{
		Record *n = pOrder->head;
		if (n != 0)
		{
			while (n->next != 0) n = n->next;
			n->next = head;
			head = pOrder->head;
			size += pOrder->size;
		}
	}

	int size = 0;
	Record * head = 0;
};

#endif