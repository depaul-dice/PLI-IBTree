#ifndef PLIDB_h
#define PLIDB_h


#include "Queries/Types/ReturningList.h"
#include <sys/stat.h>
#include <string>
#include "IBPlusTree/IBPlusTree.h"
#include "PLITypes/Data.h"


class PLIDB
{
	bool changed = false;
	IBPlusTree * iBPlusTree;
	std::string keyType = "";
	std::string dnName = "";
	

public:
	PLIDB(const std::string& name);
	~PLIDB();

	void SetKeyType(KeyType KeyType);
	int Put(
		const Data& key,
		const std::string& value);
	int Get(
		const Interval & interval,
		ReturningList* values, int* FileAccess, int* Ft);

	static void CreateFolder(std::string ibPlusDataBase);
};
#endif


