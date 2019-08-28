#if defined(_WIN32) || defined(_WIN64)
//It includes this library when the application runs on a windows operating system.
#include <windows.h>
#endif

#include "PLIDB.h"


constexpr unsigned int hash(const char *s, int off = 0) {
	return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
}


//This function is used to instantiate the database system with a parameter as a string to the directory where the database is located.
PLIDB::PLIDB(const std::string& dbname)
{
	{
		dnName= dbname + "-PLI";
		CreateFolder(dnName);
		iBPlusTree = new IBPlusTree(dnName);
		struct stat buffer;
		//Verify if the database already exists?
		if (stat((dnName + "/" + "ibPlusTreeMD.dat").c_str(), &buffer) != 0)
		{
			//This is when the database does not previously exists and it need to be constructed with its historical initialization.
			std::ifstream fin1("./listBuckets_sorted_2.txt");
			Interval interval;
			interval.start = 0.0;
			interval.end = 0.0;
			int bucketID = 0;
			IBTree* tree = new IBTree(dnName, iBPlusTree->threadManager);
			int count = 0;
			int number = 50000;
			std::string line;
			bool b = false;
			int i = 0;
			Interval interval2;
			while (std::getline(fin1, line))
			{
				std::vector<std::string> tokens = StringFunctions::Split(line, " ");
				if (tokens[0] == "interval")
				{
					if (count >= number) break;
					i = bucketID = -std::stoi(tokens[1].replace(tokens[1].find_first_of(':'), 1, ""));
					BucketInfo bi;
					bi.offset = bucketID;
					interval.start = std::stof(tokens[2]);
					interval.end = std::stof(tokens[3]);
					if (bucketID == 0)
					{
						b = true;
						interval2.start = interval.start;
						interval2.end = interval.end;
						continue;
					}
					tree->insertBucket(interval, bi);
					count += 1;
				}
			}
			if (b)
			{
				BucketInfo bi;
				bi.offset = i - 1;
				tree->insertBucket(interval2, bi);
			}
			fin1.close();
			iBPlusTree->setIBTree1(tree);
			iBPlusTree->copyStructure(tree);
		}
		else
		{
			//This is when the database already has been constructed and  is filled with data.

			std::string STRING;
			std::ifstream infile;
			infile.open(dnName + "/" + "Config");
			getline(infile, STRING);
			infile.close();



			switch (hash(STRING.c_str()))
			{
			case hash("integer"): keyType = typeid(int).name();
				Data::kt = KeyType::Integer;
				break;
			case hash("double"): keyType = typeid(double).name();
				Data::kt = KeyType::Double;
				break;
			case hash("float"): keyType = typeid(float).name();
				Data::kt = KeyType::Float;
				break;
			case hash("class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"): keyType = typeid(std::string).name();
				Data::kt = KeyType::String;
			}

			iBPlusTree->read();
		}
	}
}



void PLIDB::SetKeyType(KeyType KeyType)
{
	//std::cout << "set1\n";
	//std::cout << iBPlusTree->rootNode->length;
	struct stat buffer;
	if (stat((dnName + "/" + "ibPlusTreeMD.dat").c_str(), &buffer) != 0)
	//if (iBPlusTree->rootNode->length== 0)
	{
		{
			//std::cout << "set\n";
			Data::kt = KeyType;
			//typeid(int);
			//const std::type_info& ktt = typeid(int);
			/*const char* str = typeid(int).name();
			std::cout << str << "\n";
			const char* str2 = typeid(float).name();
			std::cout << str2 << "\n";
			const char* str3 = typeid(double).name();
			std::cout << str3 << "\n";
			const char* str4 = typeid(std::string).name();
			std::cout << str4 << "\n";*/
			switch (Data::kt)
			{
			case Integer: keyType = std::string(typeid(int).name());
				break;
			case Double: keyType = std::string(typeid(double).name());
				break;
			case Float: keyType = std::string(typeid(float).name());
				break;
			case String: keyType = std::string(typeid(std::string).name());
			}
			changed = true;

		}
	}
	//else
	{
		//std::cout << "The type of the key is already selected.\n";
	}
}


//This function is called when the database gets destructed.
PLIDB::~PLIDB()
{
	{
		if (changed)
		{
			iBPlusTree->save();
		if (keyType != "")
		{
			std::ofstream* fout = new std::ofstream(dnName + "/" + "Config", std::ofstream::out);
			(*fout) << keyType;
			fout->close();
		}
		}
	}
}


//This function is used ti insert a key with its value into the database.
int PLIDB::Put(const Data& key, const std::string& value)
{
	{
		changed = true;
		int result = iBPlusTree->insertTuple(key, (char*)value.c_str());
		return result;
	}
}


//This function is used to search the database on a interval(the first parameter).
//It returns the tuples through the second parameter(ListTUples)
//The third parameter is th number of files, read and searched only to find the tuples being in the specified range.
//The fourth parameter is the total number of files, read during the search that is happening for the threads in the concurrency queue. 
int PLIDB::Get(
	const Interval & interval,
	ReturningList* values, int* FileAccess, int* Ft)
{
	{
		std::string output;
		return iBPlusTree->search(values, interval,  FileAccess, Ft);
	}
}

//This function is used to create folder on the path given by first parameter.
void PLIDB::CreateFolder(std::string ibPlusDataBase)
{
	std::string x = ".//" + ibPlusDataBase;
#ifdef _WIN32 || _WIN64
	DWORD ftyp = GetFileAttributesA(ibPlusDataBase.c_str());
	if ((ftyp & FILE_ATTRIBUTE_DIRECTORY))
		CreateDirectory(x.c_str(), NULL);
#elif __linux__
	const int dir_err = mkdir(ibPlusDataBase.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}
