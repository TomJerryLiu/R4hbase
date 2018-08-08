#include"HConnection.h"
using namespace apache::hadoop::hbase::thrift2;
HConnection::HConnection(const char *server, const char *port):
			socket(new TSocket(server, atoi(port))),
			transport(new TBufferedTransport(socket)),
			protocol(new TBinaryProtocol(transport)),
			client(protocol)
			{
			_is_connected = false;
}

HConnection::~HConnection() {

}
//获得连接
bool HConnection::connect(){
	      _is_connected = false;
	     try {
		     transport->open();
		     _is_connected = true;
		     cout<<"connection to thrift2 established"<<endl;
			} catch (const TException &tx) {
			std::cerr << "ERROR: " << tx.what() << std::endl;
			}
	        return _is_connected;
	      }
bool HConnection::isconnect(){
		return _is_connected;
}
bool HConnection::disconnect() {
	if(_is_connected){
  try {
    transport->close();
    _is_connected = false;
    cout<<"the connection is cut down..."<<endl;
    return true;
  } catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
    return false;
  }
	}
	return 0;
}

bool HConnection::reconnect() {
  bool flag = disconnect();
  flag = flag & connect();
  return flag;
}
/*bool exists(
     the table to check on
    1: required binary table,
    2: required TGet tget
  ) throws (1:TIOError io)*/
bool HConnection::exist(const std::string& table,const TGet& tget){
try{
    bool exs = client.exists(table,tget);
    return exs;
    }
    catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
    return false;
  }
}
int32_t HConnection::openScanner(const std::string& table, const TScan& tscan){
try{
    int32_t exs = client.openScanner(table,tscan);
    return exs;
    }
    catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
    return -1;
  }
}
void HConnection::getScannerRows(std::vector<TResult> & _return, const int32_t scannerId, const int32_t numRows){
try{
    client.getScannerRows(_return,scannerId,numRows);
    }
    catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
  }
}
void HConnection::closeScanner(const int32_t scannerId){
try{
    client.closeScanner(scannerId);
    }
    catch (const TException &tx) {
    std::cerr << "ERROR: " << tx.what() << std::endl;
  }
}
void HConnection::put(const std::string& table,const TPut& tput){
try{
client.put(table,tput);
}
catch (const TIOError &io){
std::cerr << "ERROR: " << io.what() << std::endl;
}
}
void HConnection::putMultiple(const std::string& table,const std::vector<TPut>& puts){
try{
    client.putMultiple(table,puts);
}
catch (const TIOError &io){
    std::cerr << "ERROR: " << io.what() << std::endl;}
}
void HConnection::get(TResult& tresult,const std::string& table,const TGet& get){

    try{
        client.get(tresult,table,get);}
    catch (const TIOError &io){
    std::cerr << "ERROR: " << io.what() << std::endl;}
}

void HConnection::_delete(const std::string& table, const TDelete& tdelete){
		try{
			client.deleteSingle(table,tdelete);
		}
		catch (const TIOError &io){
		    std::cerr << "ERROR: " << io.what() << std::endl;
		}
}
void HConnection::getMultiple(std::vector<TResult> & results, const std::string& table,const  std::vector<TGet> & tgets){
 try{
    client.getMultiple(results,table,tgets);
    }
    catch (const TIOError &io){
    std::cerr << "ERROR: " << io.what() << std::endl;}
}
