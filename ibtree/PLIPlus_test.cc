
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <cassert>
#include <vector>
#include <inttypes.h>
#include <iostream>

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include "tree/tools/StringFunctions.h"
#include "tree/PLITypes/Constants.h"
#include "tree/IBTree/Types/IBEntry.h"
#include "tree/IBTree/Types/IBNode.h"
#include "tree/IBPlusTree/Types/IBPlusNode.h"
#include "tree/IBTree/IBTree.h"
#include "tree/IBPlusTree/IBPlusTree.h"
#include "tree/tools/FloatingPoint.h"
#include "tree/Queries/Types/ReturningList.h"

#include <postgresql/libpq-fe.h>
#include <pqxx/pqxx>


#include <chrono>

#include <cstdlib>


#include "tree/PLIDB.h"

#include "tree/tools/timeTools.h"

#include <thread>
//using namespace std;

class pair1
{
	public:
		float key;
		std::string value;
};


bool myfunction (pair1* f,pair1* s) { return (f->key<s->key); }



void Insert(PLIDB* db, int count)
{
    int splitBy=2000000;
    int c=count/splitBy;


	std::vector<pair1*> v;
	v.reserve(splitBy);

    int fetchedrows=0;

	timer* ins_time = new timer();
	timer* load_time = new timer();
	timer* total_time = new timer();
	timer* buf_load = new timer();
	buf_load->start(); buf_load->end();

	load_time->start(); load_time->end();
	ins_time->start();	ins_time->end();

	total_time->start();
	//float min, max;
    for(int j=0;j<c;j++)
    {
    	//Load data from database...
        load_time->reStartValue(load_time->getResult());
    	PGconn *conn = PQconnectdb("dbname=nyc-taxi-data user=mo3026");
    	if (PQstatus(conn) == CONNECTION_BAD)
    	{
			puts("We were unable to connect to the database");
			exit(0);

    	}
    	PGresult* res = PQexec(conn,(std::string(
    			"select trip_distance, r::text, ROW_NUMBER() over() as id from (select * from trips order by id) AS r where id>= " +
				std::to_string(j * splitBy) + " fetch first " + std::to_string(splitBy) + " rows only")).c_str());


		if (PQresultStatus(res) != PGRES_TUPLES_OK)
		{
			puts("We did not get any data!");
			exit(0);
		}

		count= PQntuples(res);
		printf("%i: We received %d records beginning with: %s\n",j , count, PQgetvalue(res, 0, 1));
		fetchedrows+=count;
		//Load data to buffer...
		buf_load->reStartValue(buf_load->getResult());
		int i = 0;
		for (i = 0; i < count; i++)
		{
			delete v[i];
		}
		v.clear();
		for (i = 0; i < splitBy; i++)
		{
			pair1* pp=new pair1();
			pp->key=FloatingPoint::ToFloat(PQgetvalue(res, i, 0));
			pp->value=std::string(PQgetvalue(res, i, 1));
			v.push_back(pp);
		}
		PQclear(res);
		PQfinish(conn);
		buf_load->end();
		load_time->end();

		ins_time->reStartValue(ins_time->getResult());
		for(int i=0;i<v.size();i++)
		{
			db->Put(v[i]->key, v[i]->value);
			std::cout<<v[i]->value<<"\n";
		}
		ins_time->end();
    }
    total_time->end();

    printf("We received a total of %d records\n", fetchedrows);
    std::cout << "Loading time:\t" << load_time->getResult() << " seconds\n";
    std::cout << "\tBuffer loading:\t" << buf_load->getResult() << " seconds\n";
    std::cout << "Insertion time:\t" << ins_time->getResult() << " seconds\n";
    std::cout << "Total time:\t" << total_time->getResult() << " seconds\n";
    printf("===============================================================\n");

}



std::mutex h;
void search(PLIDB* db, float min, float max)
{
	std::cout << "\n\n";
	std::cout << "Search the data based on the internal that covers the minimum and the maximum of the data, inserted\n\n";
	ReturningList* listTuples = new ReturningList();

	std::string output;
	int fa=0;
	int ft=0;
	timer* query_time = new timer();
	timer* verify_time = new timer();
	timer* total_time = new timer();
	total_time->start();

	//Querying data...
	query_time->start();
	db->Get(Interval(min, max),listTuples, &fa, &ft);
	query_time->end();


	//verify...
	bool b=true;
	bool duplicates=false;



	total_time->end();

	//Printing....
	h.lock();
	std::cout << "Searched Interval:"<< "\t\t\t["<<min<<","<<max<<"]\n";
	std::cout << "Number of returned tuples:"<< "\t\t"<<listTuples->size<<"\n";
	std::cout << "Are all the tuples in the range:"<< "\t"<<std::boolalpha<<b<<"\n";
	std::cout << "Are there any duplicate tuples:"<< "\t\t"<<std::boolalpha<<duplicates<<"\n";
	std::cout << "The number of files Scanned:"<< "\t\t"<<fa<<"\n";
	std::cout << "The number of file access:"<< "\t\t"<<ft<<"\n";

	printf("====== Execution time:\n");
    std::cout << "\t Querying time:\t" << query_time->getResult() << " seconds\n";
    std::cout << "\t Verifying time:\t" << verify_time->getResult() << " seconds\n";
    std::cout << "\t Total time:\t" << total_time->getResult() << " seconds\n";
    printf("===================================================================\n\n");
    h.unlock();

	delete listTuples;
	listTuples=0;
}


#include <random>

std::random_device rd;
std::mt19937 eng(rd());

void InsertFromAnywhere(PLIDB* db, int count)
{
	PGconn *conn = PQconnectdb("dbname=nyc-taxi-data user=tonhai");
	if (PQstatus(conn) == CONNECTION_BAD){
		puts("We were unable to connect to the database");
		exit(0);
	}
	std::vector<pair1*> v;
	v.reserve(count);

	std::uniform_int_distribution<> startingPoint(0, 1000000);
    for(int j=0;j<1;j++)
    {
		int st = startingPoint(eng);
    	PGconn *conn = PQconnectdb("dbname=nyc-taxi-data user=tonhai");
    	if (PQstatus(conn) == CONNECTION_BAD){
			puts("We were unable to connect to the database");
			exit(0);
    	}
    	PGresult* res = PQexec(conn,(std::string("select trip_distance, r::text, ROW_NUMBER() over() as id from trips AS r where id>= "+std::to_string(st)+" fetch first "+std::to_string(count)+" rows only")).c_str());

    	h.lock();
    	std::cout<<"\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tInserting "<<count<< " rows to the database.\n";
    	h.unlock();
		if (PQresultStatus(res) != PGRES_TUPLES_OK){
			puts("We did not get any data!");
			exit(0);
		}
		count= PQntuples(res);

		int i = 0;
		for (; i < count; i++){
			pair1* pp=new pair1();
			pp->key=FloatingPoint::ToFloat(PQgetvalue(res, i, 0));
			pp->value=std::string(PQgetvalue(res, i, 1));
			v.push_back(pp);
		}
		PQclear(res);
		PQfinish(conn);

		//std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		for(int i=0;i<v.size();i++){
			db->Put(v[i]->key, v[i]->value);
		}

		for (; i < count; i++){
			delete v[i];
		}
		v.clear();
    }
}


int NumOfDone=0;

void process(PLIDB* db,std::string b, int threadID)
{
	std::vector<std::string> tokens = StringFunctions::Split(b, "),(");
	for(int i=0; i<tokens.size(); i++)
	{
		std::vector<std::string> tokens2 = StringFunctions::Split(tokens[i], ",");
		std::string beg=tokens2[0][0]=='('? tokens2[0].substr(1,tokens2[0].size()-1):tokens2[0];
		std::string end=tokens2[1];
		float a=std::stof(beg);
		float b=std::stof(end);
		if(!(FloatingPoint::almostEqual(a,0) && FloatingPoint::almostEqual(b,0))){
			if(FloatingPoint::almostEqual(a,0) && b<0){
				std::this_thread::sleep_for(std::chrono::milliseconds((int)-b));
			}
			else{
				if(a<0 && b>0){
					InsertFromAnywhere(db,b);
				}
				else{
					search(db,a,b);
				}
			}
		}
	}
	h.lock();
    std::cout<<"\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tThread "<<threadID<<" is Done.\n"<<std::flush;
    NumOfDone++;
	h.unlock();
}

std::uniform_int_distribution<> count(1, 20);
std::uniform_real_distribution<float> first(0, 30);
std::uniform_real_distribution<float> second(0.01, 0.2);

std::uniform_int_distribution<> nonQueryIndex(0, 9);
std::uniform_int_distribution<> silenceAmount(1, 10);

void CreateTestFilesQuery()
{
	PLIDB::CreateFolder("./");

	for (int i = 1; i<31; i++)
	{
		std::ofstream* fout = new std::ofstream(std::string("./TestFilesQ") + std::string("/Test-") + std::to_string(i), std::ofstream::out);
		//(*fout) << std::to_string( mainWrittingBuffer->fNumber)+","+ std::to_string(mainWrittingBuffer->size)+"\n";
		//(fout)->write( mainWrittingBuffer->data, mainWrittingBuffer->size);


		int c = count(eng);
		int indexNonQuery = 0;
		float a, b;
		for (int j = 0; j<c; j++)
		{
			if (j % 9 == 0) indexNonQuery = nonQueryIndex(eng) + j / 9;

				if (j == indexNonQuery)
				{
					a = 0.0;
					b = (float)-silenceAmount(eng);
				}
				else
				{
					a = first(eng);
					b = a + second(eng);
				}
				(*fout) << "(" << std::to_string(a) + "," + std::to_string(b) + "),";
				if (j%9 == 0) indexNonQuery = nonQueryIndex(eng) + j / 9;
		}
		a = first(eng);
		b = a + second(eng);
		(*fout) << "(" << std::to_string(a) + "," + std::to_string(b) + ")";
		fout->close();
	}
}

void CreateTestFilesQI()
{
	PLIDB::CreateFolder("./TestFilesQI");

	for (int i = 1; i<31; i++)
	{
		std::ofstream* fout = new std::ofstream(std::string("./TestFilesQI") + std::string("/Test-") + std::to_string(i), std::ofstream::out);

		std::uniform_int_distribution<> insertORsilence(1, 2);
		std::uniform_int_distribution<> insertionType(1, 5);

		std::uniform_int_distribution<> mediumInsertion(2, 10);
		std::uniform_int_distribution<> largeInsertion(11, 100);

		int c = count(eng);
		int indexNonQuery=0;
		float a, b;
		for (int j = 0; j<c; j++)
		{
			if (j % 9 == 0) indexNonQuery = nonQueryIndex(eng) + j / 9;

				if (j == indexNonQuery)
				{
					int insert = insertORsilence(eng);
					if (insert == 1)
					{
						a = -1;
						int type = insertionType(eng);
						switch (type)
						{
						case 4:
							b=mediumInsertion(eng);
							break;
						case 5:
							b=largeInsertion(eng);
							break;
						default: b=1;
						}

					}
					else
					{
						a = 0.0;
						b = (float)-silenceAmount(eng);
					}
				}
				else
				{
					a = first(eng);
					b = a + second(eng);
				}
				(*fout) << "(" << std::to_string(a) + "," + std::to_string(b) + "),";
		}
		a = first(eng);
		b = a + second(eng);
		(*fout) << "(" << std::to_string(a) + "," + std::to_string(b) + ")";
		fout->close();
	}
}

std::mutex fileLock;
std::thread* CreateRandonThread(PLIDB* db, std::string RootFolderTestFiles, int i)
{
	std::thread t1([](PLIDB* db, std::string RootFolderTestFiles, int i) {

	    std::uniform_int_distribution<> distr(1, 30);
		std::string z=RootFolderTestFiles + std::string("/Test-") + std::to_string(distr(eng));

		h.lock();
		std::cout<<i<<": "<<z<<"\n";
		h.unlock();

		fileLock.lock();
		std::ifstream inFile;
		inFile.open(z);//open the input file

		std::stringstream strStream;
		strStream << inFile.rdbuf();
		std::string str = strStream.str();
		inFile.close();
		fileLock.unlock();

		process(db,str, i);

		}, db, RootFolderTestFiles, i);
	t1.detach();
	return &t1;
}

void testQuery(float a, float b){
	PLIDB* db;
	//Clear OS cache...
	std::system("echo 'TonHai1111' | sudo -S sh -c 'echo 1 > /proc/sys/vm/drop_caches'");
	std::system("echo 'TonHai1111' | sudo -S sh -c 'echo 2 > /proc/sys/vm/drop_caches'");
	//std::string command1 = "./clearCache1.sh";
	//std::string command2 = "./clearCache2.sh";
	//std::system(command1.c_str());
	//std::system(command2.c_str());
	timer* init_time = new timer();
	//Initialization...
	init_time->start();
	db= new PLIDB("taxi146");
	init_time->end();
	printf("====== Initial time:\n");
	std::cout << "\t Initial time:\t" << init_time->getResult() << " seconds\n";
	printf("===================================================================\n\n");
	//Querying data...
	search(db, a, b);

	delete db;
}

void threadRunner(PLIDB* db, float queries[][2], int row, int NUM_QUERIES, int NUM_THREADS, int threadID){
	int i = 0;
	std::cout<<"\nThread "<<threadID<<" is running.\n"<<std::flush;
	for (i = 0; i < NUM_QUERIES; i++){
		if((i % NUM_THREADS) == row){
			search(db, queries[i][0], queries[i][1]);
		}
	}
	//h.lock();
	std::cout<<"\nThread "<<threadID<<" is Done.\n"<<std::flush;
	//h.unlock();
}

void concurrenceTest()
{
	std::cout.setf(std::ios::unitbuf);
	PLIDB* db;
	//Clear OS cache...
	std::system("echo 'TonHai1111' | sudo -S sh -c 'echo 1 > /proc/sys/vm/drop_caches'");
	std::system("echo 'TonHai1111' | sudo -S sh -c 'echo 2 > /proc/sys/vm/drop_caches'");

	timer* init_time = new timer();
	//Querying data...
	init_time->start();
	db= new PLIDB("taxi146");
	init_time->end();

	printf("====== Initial time:\n");
	std::cout << "\t Initial time:\t" << init_time->getResult() << " seconds\n";
	printf("===================================================================\n\n");

	timer* query_time = new timer();
	query_time->start();

	//List of queries:

	int const NUM_QUERIES = 40;
	std::string strQueries = "";
			//"0.0,0.1; 1.1,1.2; 2.2,2.3; 3.3,3.4; 3.0,3.1; " +
			//"4.1,4.2; 5.2,5.3; 6.3,6.4; 6.0,6.1; 7.1,7.2;" +
			//"8.2,8.3; 9.3,9.4; 0.0,0.1; 4.1,4.2; 8.2,8.3;" +
			//"3.3,3.4; 4.4,4.5; 5.5,5.6; 6.6,6.7; 7.7,7.8;" +
			//"7.4,7.5; 8.5,8.6; 9.6,9.7; 0.7,0.8; 0.4,0.5;" +
			//"1.5,1.6; 2.6,3.7; 3.7,3.8; 7.4,7.5; 1.5,1.6;" +
			//"6.6,6.7; 0.7,0.8; 8.8,8.9; 9.9,10.0; 1.8,1.9;" +
			//"2.9,3.0; 4.8,4.9; 5.9,6.0; 4.8,4.9; 9.9,10.0";
	float Queries[NUM_QUERIES][2] = {
			{0.0,0.1}, {1.1,1.12}, {2.2,2.23}, {3.3,3.34}, {3.0,3.04},
			{4.1,4.12}, {5.2,5.23}, {6.3,6.34}, {6.0,6.01}, {7.1,7.12},
			{8.2,8.23}, {9.3,9.34}, {0.0,0.01}, {4.1,4.12}, {8.2,8.23},
			{3.3,3.34}, {4.4,4.45}, {5.5,5.56}, {6.6,6.67}, {7.7,7.78},
			{7.4,7.45}, {8.5,8.56}, {9.6,9.67}, {0.7,0.71}, {0.4,0.41},
			{1.5,1.51}, {2.6,2.67}, {3.7,3.73}, {7.4,7.45}, {1.5,1.51},
			{6.6,6.67}, {0.7,0.71}, {8.8,8.84}, {9.9,10.0}, {1.8,1.81},
			{2.9,2.91}, {4.8,4.82}, {5.9,5.92}, {4.8,4.82}, {9.9,10.0}
	};
	/*float Queries[NUM_QUERIES][2] = {
				{0.0,0.1}, {1.1,1.2}, {2.2,2.3}, {3.3,3.4}, {3.0,3.1},
				{4.1,4.2}, {5.2,5.3}, {6.3,6.4}, {6.0,6.1}, {7.1,7.2},
				{8.2,8.3}, {9.3,9.4}, {0.0,0.1}, {4.1,4.2}, {8.2,8.3},
				{3.3,3.4}, {4.4,4.5}, {5.5,5.6}, {6.6,6.7}, {7.7,7.8},
				{7.4,7.5}, {8.5,8.6}, {9.6,9.7}, {0.7,0.8}, {0.4,0.5},
				{1.5,1.6}, {2.6,3.7}, {3.7,3.8}, {7.4,7.5}, {1.5,1.6},
				{6.6,6.7}, {0.7,0.8}, {8.8,8.9}, {9.9,10.0}, {1.8,1.9},
				{2.9,3.0}, {4.8,4.9}, {5.9,6.0}, {4.8,4.9}, {9.9,10.0}
	};*/
	/*//1 thread
	int const NUM_THREADS = 1;
	std::thread th1 (threadRunner, db, Queries, 0, NUM_QUERIES, NUM_THREADS, 1);
	th1.join();*/
	/*//2 threads
	int const NUM_THREADS = 2;
	std::thread th1 (threadRunner, db, Queries, 0, NUM_QUERIES, NUM_THREADS, 1);
	std::thread th2 (threadRunner, db, Queries, 1, NUM_QUERIES, NUM_THREADS, 2);
	th1.join();
	th2.join();*/

	/*//4 threads
	int const NUM_THREADS = 4;
	std::thread th1 (threadRunner, db, Queries, 0, NUM_QUERIES, NUM_THREADS, 1);
	std::thread th2 (threadRunner, db, Queries, 1, NUM_QUERIES, NUM_THREADS, 2);
	std::thread th3 (threadRunner, db, Queries, 2, NUM_QUERIES, NUM_THREADS, 3);
	std::thread th4 (threadRunner, db, Queries, 3, NUM_QUERIES, NUM_THREADS, 4);
	th1.join();
	th2.join();
	th3.join();
	th4.join();*/
	/*//8 threads
	int const NUM_THREADS = 8;
	std::thread th1 (threadRunner, db, Queries, 0, NUM_QUERIES, NUM_THREADS, 1);
	std::thread th2 (threadRunner, db, Queries, 1, NUM_QUERIES, NUM_THREADS, 2);
	std::thread th3 (threadRunner, db, Queries, 2, NUM_QUERIES, NUM_THREADS, 3);
	std::thread th4 (threadRunner, db, Queries, 3, NUM_QUERIES, NUM_THREADS, 4);
	std::thread th5 (threadRunner, db, Queries, 4, NUM_QUERIES, NUM_THREADS, 5);
	std::thread th6 (threadRunner, db, Queries, 5, NUM_QUERIES, NUM_THREADS, 6);
	std::thread th7 (threadRunner, db, Queries, 6, NUM_QUERIES, NUM_THREADS, 7);
	std::thread th8 (threadRunner, db, Queries, 7, NUM_QUERIES, NUM_THREADS, 8);
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
	th6.join();
	th7.join();
	th8.join();*/
	//16 threads
	int const NUM_THREADS = 16;
	std::thread th1 (threadRunner, db, Queries, 0, NUM_QUERIES, NUM_THREADS, 1);
	std::thread th2 (threadRunner, db, Queries, 1, NUM_QUERIES, NUM_THREADS, 2);
	std::thread th3 (threadRunner, db, Queries, 2, NUM_QUERIES, NUM_THREADS, 3);
	std::thread th4 (threadRunner, db, Queries, 3, NUM_QUERIES, NUM_THREADS, 4);
	std::thread th5 (threadRunner, db, Queries, 4, NUM_QUERIES, NUM_THREADS, 5);
	std::thread th6 (threadRunner, db, Queries, 5, NUM_QUERIES, NUM_THREADS, 6);
	std::thread th7 (threadRunner, db, Queries, 6, NUM_QUERIES, NUM_THREADS, 7);
	std::thread th8 (threadRunner, db, Queries, 7, NUM_QUERIES, NUM_THREADS, 8);
	std::thread th9 (threadRunner, db, Queries, 8, NUM_QUERIES, NUM_THREADS, 9);
	std::thread th10 (threadRunner, db, Queries, 9, NUM_QUERIES, NUM_THREADS, 10);
	std::thread th11 (threadRunner, db, Queries, 10, NUM_QUERIES, NUM_THREADS, 11);
	std::thread th12 (threadRunner, db, Queries, 11, NUM_QUERIES, NUM_THREADS, 12);
	std::thread th13 (threadRunner, db, Queries, 12, NUM_QUERIES, NUM_THREADS, 13);
	std::thread th14 (threadRunner, db, Queries, 13, NUM_QUERIES, NUM_THREADS, 14);
	std::thread th15 (threadRunner, db, Queries, 14, NUM_QUERIES, NUM_THREADS, 15);
	std::thread th16 (threadRunner, db, Queries, 15, NUM_QUERIES, NUM_THREADS, 16);
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
	th6.join();
	th7.join();
	th8.join();
	th9.join();
	th10.join();
	th11.join();
	th12.join();
	th13.join();
	th14.join();
	th15.join();
	th16.join();

	/*//32 threads
	int const NUM_THREADS = 32;
	std::thread th1 (threadRunner, db, Queries, 0, NUM_QUERIES, NUM_THREADS, 1);
	std::thread th2 (threadRunner, db, Queries, 1, NUM_QUERIES, NUM_THREADS, 2);
	std::thread th3 (threadRunner, db, Queries, 2, NUM_QUERIES, NUM_THREADS, 3);
	std::thread th4 (threadRunner, db, Queries, 3, NUM_QUERIES, NUM_THREADS, 4);
	std::thread th5 (threadRunner, db, Queries, 4, NUM_QUERIES, NUM_THREADS, 5);
	std::thread th6 (threadRunner, db, Queries, 5, NUM_QUERIES, NUM_THREADS, 6);
	std::thread th7 (threadRunner, db, Queries, 6, NUM_QUERIES, NUM_THREADS, 7);
	std::thread th8 (threadRunner, db, Queries, 7, NUM_QUERIES, NUM_THREADS, 8);
	std::thread th9 (threadRunner, db, Queries, 8, NUM_QUERIES, NUM_THREADS, 9);
	std::thread th10 (threadRunner, db, Queries, 9, NUM_QUERIES, NUM_THREADS, 10);
	std::thread th11 (threadRunner, db, Queries, 10, NUM_QUERIES, NUM_THREADS, 11);
	std::thread th12 (threadRunner, db, Queries, 11, NUM_QUERIES, NUM_THREADS, 12);
	std::thread th13 (threadRunner, db, Queries, 12, NUM_QUERIES, NUM_THREADS, 13);
	std::thread th14 (threadRunner, db, Queries, 13, NUM_QUERIES, NUM_THREADS, 14);
	std::thread th15 (threadRunner, db, Queries, 14, NUM_QUERIES, NUM_THREADS, 15);
	std::thread th16 (threadRunner, db, Queries, 15, NUM_QUERIES, NUM_THREADS, 16);
	std::thread th17 (threadRunner, db, Queries, 16, NUM_QUERIES, NUM_THREADS, 17);
	std::thread th18 (threadRunner, db, Queries, 17, NUM_QUERIES, NUM_THREADS, 18);
	std::thread th19 (threadRunner, db, Queries, 18, NUM_QUERIES, NUM_THREADS, 19);
	std::thread th20 (threadRunner, db, Queries, 19, NUM_QUERIES, NUM_THREADS, 20);
	std::thread th21 (threadRunner, db, Queries, 20, NUM_QUERIES, NUM_THREADS, 21);
	std::thread th22 (threadRunner, db, Queries, 21, NUM_QUERIES, NUM_THREADS, 22);
	std::thread th23 (threadRunner, db, Queries, 22, NUM_QUERIES, NUM_THREADS, 23);
	std::thread th24 (threadRunner, db, Queries, 23, NUM_QUERIES, NUM_THREADS, 24);
	std::thread th25 (threadRunner, db, Queries, 24, NUM_QUERIES, NUM_THREADS, 25);
	std::thread th26 (threadRunner, db, Queries, 25, NUM_QUERIES, NUM_THREADS, 26);
	std::thread th27 (threadRunner, db, Queries, 26, NUM_QUERIES, NUM_THREADS, 27);
	std::thread th28 (threadRunner, db, Queries, 27, NUM_QUERIES, NUM_THREADS, 28);
	std::thread th29 (threadRunner, db, Queries, 28, NUM_QUERIES, NUM_THREADS, 29);
	std::thread th30 (threadRunner, db, Queries, 29, NUM_QUERIES, NUM_THREADS, 30);
	std::thread th31 (threadRunner, db, Queries, 30, NUM_QUERIES, NUM_THREADS, 31);
	std::thread th32 (threadRunner, db, Queries, 31, NUM_QUERIES, NUM_THREADS, 32);
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
	th6.join();
	th7.join();
	th8.join();
	th9.join();
	th10.join();
	th11.join();
	th12.join();
	th13.join();
	th14.join();
	th15.join();
	th16.join();
	th17.join();
	th18.join();
	th19.join();
	th20.join();
	th21.join();
	th22.join();
	th23.join();
	th24.join();
	th25.join();
	th26.join();
	th27.join();
	th28.join();
	th29.join();
	th30.join();
	th31.join();
	th32.join();*/

	query_time->end();
	std::cout << "\t Querying time:\t" << query_time->getResult() << " seconds\n";


	delete db;
	std::cout << "\n";
	std::cout<<"Done\n"<<std::flush;
}
int main(int argc, char** argv)
{
	//testQuery(3.9, 5); //10%
	//testQuery(4.4, 5); //5.2%
	//testQuery(4.6, 5); //3%
	//testQuery(4.9, 5); //1.1%
	//testQuery(4.95, 5); //0.5%
	//concurrenceTest();



    //std::cout.setf(std::ios::unitbuf);
    PLIDB* db;
    //if(argc>3)
    //{
    //	db= new PLIDB(argv[1],0);
    //	//Insert(db,146000000);
    //	search(db,std::atof(argv[2]),std::atof(argv[3]));
    //}
    //else
    //{
    //	timer* init_time = new timer();
    //	//Querying data...
    //	init_time->start();
		db= new PLIDB("taxi146");
		db->SetKeyType(KeyType::Float);
    //	init_time->end();
		//Insert(db,4000000);

		std::system("sudo sh -c 'echo 1 > /proc/sys/vm/drop_caches'");
		std::system("sudo sh -c 'echo 2 > /proc/sys/vm/drop_caches'");
		search(db, 3.9, 5); //10%
//		printf("====== Initial time:\n");
//	    std::cout << "\t Initial time:\t" << init_time->getResult() << " seconds\n";
//	    printf("===================================================================\n\n");

		timer* query_time = new timer();
		query_time->start();


		/*int i=1;
		std::string rootFolderTestFiles="./TestFilesQ";
		std::thread t1([](PLIDB* db, std::string RootFolderTestFiles, int i) {

			process(db,
					"(0.0,0.1),(1.1,1.2),(7.4,7.5),(8.5,8.6),(8.2,8.3),(9.3,9.4),(6.6,6.7),(0.7,0.8),(5.2,5.3),(6.3,6.4),(2.6,3.7),(3.7,3.8),(4.4,4.5),(5.5,5.6),(4.8,4.9),(5.9,6.0),(3.0,3.1),(4.1,4.2),(0.4,0.5),(1.5,1.6),(2.2,2.3),(3.3,3.4),(9.6,9.7),(0.7,0.8),(0.0,0.1),(4.1,4.2),(8.8,8.9),(9.9,10.0),(6.0,6.1),(7.1,7.2),(7.4,7.5),(1.5,1.6),(6.6,6.7),(7.7,7.8),(4.8,4.9),(9.9,10.0),(8.2,8.3),(3.3,3.4),(1.8,1.9),(2.9,3.0)"
					, i);

			}, db, rootFolderTestFiles, i);


		t1.join();*/


		//concurrenceTest();

		query_time->end();
		std::cout << "\t Querying time:\t" << query_time->getResult() << " seconds\n";


//    }

 delete db;

    std::cout << "\n";
    std::cout<<"Done\n"<<std::flush;
}






