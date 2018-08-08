/*
 * HTree.h
 *
 *  Created on: Jul 23, 2017
 *      Author: richard
 */

#ifndef INCLUDE_HTREE_H_
#define INCLUDE_HTREE_H_
#include <iostream>
#include <TObject.h>
#include <Config.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <RootEventData/TBossFullEvent.h>
#include <RootEventData/TDigiEvent.h>
#include <RootEventData/TDstEvent.h>
#include <RootEventData/TEvtHeader.h>
#include <RootEventData/TEvtRecObject.h>
#include <RootEventData/THltEvent.h>
#include <RootEventData/TMcEvent.h>
#include <RootEventData/TTrigEvent.h>
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>
#include "log4cpp/PropertyConfigurator.hh"

#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <hbaseConnect/HConnection.h>
#include <ext/hash_map>
#include <sstream>
//for thrift
#define PROPERTY_THRIFT_IP "thrift_ip"
#define DEFAULT_THRIFT_IP "127.0.0.1"
#define PROPERTY_THRIFT_PORT "thrift_port"
#define DEFAULT_THRIFT_PORT "9090"
//for hbase
#define PROPERTY_HBASE_TABLE "tableName"
#define PROPERTY_HBASE_TABLE_FAMILY "family"
//for log4cpp
#define PROPERTY_LOG_USER "username"
#define DEFAULT_LOG_USER "richard"
#define PROPERTY_LOG_PATH "log_path"
#define DEFAULT_LOG_PATH "./LOGS/"
#define PROPERTY_LOG_FILE_SIZE "log_size"
#define DEFAULT_LOG_FILE_SIZE 128*1024*1024
#define PROPERTY_LOG_FILE_SCROLL_INDEX "backupindex"
#define DEFAULT_LOG_FILE_SCROLL_INDEX 10
//for HTree
#define EVENT_CODE_SIZE 1024*128
#define PROPERTY_CACHE_EVENT_SIZE "cache_event_size"
#define DEFAULT_CACHE_EVENT_SIZE 100 //the number of events stored in cache
#define PROPERTY_PREFETCH_SIZE "prefetch_size"
#define DEFAULT_PREFETCH_SIZE 1 //if isPrefetch is true ,define how many events will be fetch at most in a scan

#define SPACE ""

using namespace std;
using namespace __gnu_cxx;

namespace __gnu_cxx {
template<> struct hash<std::string> {
    size_t operator()(const std::string& x) const {
        return hash<const char*>()(x.c_str());
    }
};
}
struct Event{
	string key;
	string BossFullEvent;
	UInt_t length;
	UInt_t d_length;
	Event *prev, *next;
};
class HTree{
public:
	HTree(Config config);
	HTree(HConnection *connection,const string& _table,const string &_family);
	HTree(){}
	void setBranchAddress(const char* branch_name);
	TEvtHeader* getTEvtHeader(){return EvtHeader;}
	TDigiEvent* getTDigiEvent(){return DigiEvent;}
	TDstEvent* getTDstEvent(){return DstEvent;}
	TMcEvent * getTMcEvent(){return McEvent;}
	TTrigEvent* getTTrigEvent(){return TrigEvent;}
	THltEvent * getTHltEvent(){return HltEvent;}
	TEvtRecObject * getTEvtRecObject(){return EvtRecObject;}
	void free();
	void getEntry(const string&rowKey,bool isPrefetch=false);
	bool exist(const string rowkey);
	void setTableProperty(const string& hTableName,const string& columnFamily);
private:
	int32_t cache_event_size;
	int32_t prefetch_event_size;
	TEvtHeader* 	EvtHeader ;
	TDigiEvent* 	DigiEvent ;
	TDstEvent * 	DstEvent ;
	TMcEvent*   	McEvent  ;
	TTrigEvent* 	TrigEvent;
	THltEvent*     	HltEvent ;
	TEvtRecObject* 	EvtRecObject;

	hash_map<string,Event*> _hashmap;
	string 			table;
	string 			family;
	HConnection *   connection;
	string username;
	vector<Event* > free_pos; // 存储可用结点的地址，事先开辟空间在新的数据来临时可以直接使用
	Event *head_,   *tail_;
	Event* event_list;
	char* code;
	char* ecode;
	enum {
		    	 hasEvtHeader     = 1<<0,
		    	 hasDigiEvent     = 1<<1,
		    	 hasDstEvent      = 1<<2,
		    	 hasMcEvent       = 1<<3,
		    	 hasTrigEvent     = 1<<4,
		    	 hasHltEvent      = 1<<5,
		    	 hasRecTrackEvent = 1<<6,
		    	 hasEvtRecObject  = 1<<7
		      };
		UInt_t          m_mask;
	void init(string thrift_ip,string thrift_port,string log_path,
			int32_t log_file_size,int32_t log_backup_index);
	void init_log(string log_path,int32_t log_file_size,int32_t log_backup_index);
	void getHbaseValues(const string& key,bool isPrefetch=false);
    // 分离结点
    void detach(Event* node);
    // 将结点插入头部
    void attach(Event* node);
};

#endif /* INCLUDE_HTREE_H_ */
