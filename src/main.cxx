/*
 * rdata.cpp
 *
 *  Created on: Jul 2, 2018
 *      Author: richard
 */
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include "RootEventData/TBossFullEvent.h"//
#include "RootEventData/TEvtRecEvent.h"
#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TDstEvent.h"
#include "RootEventData/TEvtHeader.h"
#include "RootEventData/THltEvent.h"
#include "RootEventData/TMcEvent.h"
#include "RootEventData/TTrigEvent.h"
#include "RootEventData/TEvtRecObject.h"
#include <hbaseConnect/HConnection.h>
#include <HTree.h>
#include <sstream>
#include<stdlib.h>
#include "time.h"
#include <sys/time.h>
using namespace std;
#define RAND_SIZE 100
#define BITN 8      //统一entryid的位数
double get_wall_time()
{
    struct timeval time ;
    if (gettimeofday(&time,NULL)){
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
int main(int argc,char**argv){
	char* randchar[RAND_SIZE];
	for(int i =0;i<RAND_SIZE;i++)
		randchar[i] = (char*)malloc(50*sizeof(char));
	char entryid[10];
	char data[10];

	int j ;
	int index = 0;
	for(int i = 0;i<RAND_SIZE;i++){
		sprintf(data,"%d",i);
		for (j = 0;j<BITN-strlen(data);j++)
			entryid[j]='0';
		entryid[j]='\0';
		strcat(entryid,data);
		sprintf(randchar[index++],"%s%s","09psip_stream001_run9025_file9.dst-",entryid);
		}
	Config config("./conf/htree.conf");

	config.SetProperty(Config::THRIFT_IP,"127.0.0.1");
	config.SetProperty(Config::THRIFT_PORT, 9090);
	config.SetProperty(Config::HBASE_TABLE, "TTree");
	config.SetProperty(Config::HBASE_TABLE_COLUMN_FAMILY, "Event");
	HTree* htree = new HTree(config);
	double start_time = get_wall_time();
	htree->setTableProperty("TTree", "Event");
	htree->setBranchAddress("TEvtRecObject");
	for(int i =0;i<RAND_SIZE;i++){
		htree->getEntry(randchar[i]);
		cout<<htree->getTEvtRecObject()->getEvtRecTrackCol()->GetLast()<<endl;
	}
	double end_time = get_wall_time();
	double Total_time =(end_time-start_time);
	printf( "sequential read 100 events from hbase:%f seconds\n", Total_time);
	htree->free();
	for(int i =0;i<RAND_SIZE;i++)
		free(randchar[i]);
	return 0;
}
