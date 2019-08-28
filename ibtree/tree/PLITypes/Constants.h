#ifndef Constants_h
#define Constants_h




//#!/usr/bin/python
namespace Constants
{
static const int MAX_NUM_ENTRY = 170;
static const int LOW=0;
static const int HIGH=1;

//#Return codes
static const int SUCCESS=1;
static const int ERROR1=0;


static const int OVERFLOW2=2;  //#require a split
static const int UNDERFLOW2=-1; //#require a merge

static const int SUCCESS_UPDATE_HIGH_KEY=3;
static const int SUCCESS_UPDATE_LOW_KEY=4;
static const int SUCCESS_UPDATE_MAX=5;
static const int SUCCESS_CLEAR_MAX=6;

//#Function codes
static const bool FUNC_TRUE=true;
static const bool FUNC_FALSE=false;

//#Bucket & Distances
static const float MAX_DISTANCE=99999.0;  //#MAX_HIGH
static const float MIN_DISTANCE=0.0;      //#MIN_LOW

//#IBPlusTree - Leaf node
static const int NUM_ROW_PER_BUCKET=1000;
static const int MAX_NUM_L_ENTRY=170; //#44

//#IBPlusTree - non - Leaf node
static const int MAX_NUM_NL_ENTRY=255;

//#Buffer
//static const int MAX_DATA_BUFFER=800; //#800MB
static const int MAX_DATA_BUFFER = 1800; //1400MB
static const int MAX_FLUSHING_BUFFER=200; //#200MB
static const int BUCKET_STARTING_POINT=50000; //#ID starting point

static const bool IMPRINTS_MODE=false;

static const bool IMPRINTS_MODE_NYC=false;
static const bool IMPRINTS_MODE_BD=true;
static const int IMPRINTS_NUM_PT=1000;

//#PLI
static const int MAX_SIZE_CLUSTERED=5000000;
//MAX_SIZE_CLUSTERED = 5000000

//Testing
static const int BLOCK_SIZE_3 = 400000; //2000
static const int BLOCK_SIZE = 200000; //1000
static const int BLOCK_SIZE_1 = 100000; //500 tuples / bucket
static const int BLOCK_SIZE_2 = 50000; //250 tuples / bucket

static const int BufferCapacity = 8194304;
static const int MaxFileSize = 2097152;
static const int MaxConcurrentFiles = 32;
//static const int MaxConcurrentThreads = 1000;
static const int BucketLimit = 4;
static const int MiliSecondsToWait = 200;
};

#endif
