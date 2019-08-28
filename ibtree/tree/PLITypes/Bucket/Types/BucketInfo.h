#ifndef BucketInfo_h
#define BucketInfo_h

struct BucketInfo
{
	//std::string filename="";
	int fNum = 1;
	int offset=0;
	int size=0;
	int timesItHasToBeRead=1;

	BucketInfo();
	BucketInfo(const BucketInfo& a) : fNum(a.fNum), offset(a.offset),size(a.size), timesItHasToBeRead(a.timesItHasToBeRead)  { }
};

#endif
