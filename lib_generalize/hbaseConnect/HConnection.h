#ifndef HConnection_H
#define HConnection_H
#include "gen-cpp/THBaseService.h"
#include <config.h>
#include <vector>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift2;
using boost::shared_ptr;

class HConnection{
// Constructor and Destructor
    public:
    	HConnection(const char *server, const char *port);
   	~HConnection();
   	bool connect();
    	bool disconnect();
    	bool reconnect();
    	bool isconnect();
	int32_t openScanner(const std::string& table, const TScan& tscan);
	void getScannerRows(std::vector<TResult> & _return, const int32_t scannerId, const int32_t numRows);
	void closeScanner(const int32_t scannerId);
	bool exist(const std::string& table,const TGet& tget);
	void put(const std::string& table,const TPut& tput);
	void putMultiple(const std::string& table,const std::vector<TPut>& puts);
	void get(TResult& tresult,const std::string& table,const TGet& get);
	void _delete(const std::string& table, const TDelete& tdelete);
	void getMultiple(std::vector<TResult> & results, const std::string& table,const  std::vector<TGet> & tgets);
    private:
    	boost::shared_ptr<TSocket> socket;
    	boost::shared_ptr<TTransport> transport;
    	boost::shared_ptr<TProtocol> protocol;
    	THBaseServiceClient client;
    	bool _is_connected;
};
#endif
