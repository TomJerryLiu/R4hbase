/*
 * HTree.cxx
 *
 *  Created on: Aug 11, 2017
 *      Author: richard
 */
#include<HTree.h>
#include<RootEventData/TBossFullEvent.h>
#include<utils.h>
#include<stdlib.h>
#define CACHE_SIZE 100

const int32_t numRows = 100;

HTree::HTree(Config config) {
	//read the config
	string thrift_ip;
	string thrift_port;
	string log_path;
	int32_t log_file_size;
	int32_t log_backup_index;
	try {
		thrift_ip = config.Read<string>(PROPERTY_THRIFT_IP);
	} catch (Config::Key_not_found k) {
		thrift_ip = DEFAULT_THRIFT_IP;
		cout
				<< "WARN:" + k.key
						+ " is not setted in the config file use the default value"
				<< endl;
	}
	try {
		thrift_port = config.Read<string>(PROPERTY_THRIFT_PORT);
	} catch (Config::Key_not_found k) {
		thrift_port = DEFAULT_THRIFT_PORT;
		cout
				<< "WARN:" + k.key
						+ " is not setted in the config we use the default value"
				<< endl;
	}
	try {
		log_path = config.Read<string>(PROPERTY_LOG_PATH);
	} catch (Config::Key_not_found k) {
		log_path = DEFAULT_LOG_PATH;
		cout
				<< "WARN:" + k.key
						+ " is not setted in the config we use the default value"
				<< endl;
	}
	try {
		log_file_size = config.Read<int32_t>(PROPERTY_LOG_FILE_SIZE);
	} catch (Config::Key_not_found k) {
		log_file_size = DEFAULT_LOG_FILE_SIZE;
		cout
				<< "WARN:" + k.key
						+ " is not setted in the config we use the default value"
				<< endl;
	}
	try {
		log_backup_index = config.Read<int32_t>(PROPERTY_LOG_FILE_SCROLL_INDEX);
	} catch (Config::Key_not_found k) {
		log_backup_index = DEFAULT_LOG_FILE_SCROLL_INDEX;
		cout
				<< "WARN:" + k.key
						+ " is not setted in the config we use the default value"
				<< endl;
	}
	try{
		this->cache_event_size = config.Read<int32_t>(PROPERTY_CACHE_EVENT_SIZE);
	}catch (Config::Key_not_found k) {
		this->cache_event_size = DEFAULT_CACHE_EVENT_SIZE;
		cout
				<< "WARN:" + k.key
						+ " is not setted in the config we use the default value"
				<< endl;
	}
	try{
		this->prefetch_event_size = config.Read<int32_t>(PROPERTY_PREFETCH_SIZE);
	}catch (Config::Key_not_found k) {
			this->prefetch_event_size = DEFAULT_PREFETCH_SIZE;
			cout
					<< "WARN:" + k.key
							+ " is not setted in the config we use the default value"
					<< endl;
		}
	try{
		this->username = config.Read<string>(PROPERTY_LOG_USER);
	}catch (Config::Key_not_found k) {
		this->username = DEFAULT_LOG_USER;
		cout
				<< "WARN:" + k.key
						+ " is not setted in the config we use the default value"
				<< endl;
	}
//	cout << thrift_ip << endl << thrift_port << endl << log_path << endl;
//	cout << "cache_size:" << cache_event_size << endl;
//	cout << "prefetch_size:" << prefetch_event_size << endl;
	try {
		this->table = config.Read<string>(PROPERTY_HBASE_TABLE);
		this->family = config.Read<string>(PROPERTY_HBASE_TABLE_FAMILY);
	}catch (Config::Key_not_found k){
		cout
						<< "WARN:" + k.key
								+ " is not setted in the config"
						<< endl;
	}
	m_mask = 0;
	init(thrift_ip, thrift_port, log_path, log_file_size, log_backup_index);
}
HTree::HTree(HConnection *connection, const string& _table,
		const string &_family) {
	this->connection = connection;
	table = _table;
	family = _family;
	this->head_ = new Event;
	this->tail_ = new Event;
	head_->prev = NULL;
	head_->next = tail_;
	tail_->prev = head_;
	tail_->next = NULL;
	m_mask = 0;
	code = new char[1024 * 128];
	ecode = new char[1024 * 128];
	event_list = new Event[CACHE_SIZE];
	for (int i = 0; i < CACHE_SIZE; i++) {
		this->free_pos.push_back(event_list + i);
	}
}
void HTree::init_log(string log_path, int32_t log_file_size,
		int32_t log_backup_index) {
	if(access(log_path.c_str(),R_OK|W_OK)<0){
		if(mkdir(log_path.c_str(),S_IRWXU | S_IRWXG | S_IROTH)<0){
			printf("error:%s",strerror(errno));
		}
	}
	log4cpp::PatternLayout* pLayout = new log4cpp::PatternLayout();
	pLayout->setConversionPattern("[%d{%Y-%m-%d %H:%M:%S,%l}] %p(%c): %m%n");
	log4cpp::RollingFileAppender* rollfileAppender =
			new log4cpp::RollingFileAppender("rollfileAppender",
					log_path + "BOSS_HBASE_"+username + ".log", log_file_size,
					log_backup_index);
	rollfileAppender->setLayout(pLayout);
	log4cpp::Category::getRoot().getInstance(username).addAppender(
			rollfileAppender);
	log4cpp::Category::getRoot().getInstance(username).setPriority(
			log4cpp::Priority::DEBUG);
	cout<<"logging to "+log_path+"BOSS_HBASE_"+username+".log"<<endl;
}
void HTree::init(string thrift_ip, string thrift_port, string log_path,
		int32_t log_file_size, int32_t log_backup_index) {
	this->head_ = new Event;
	this->tail_ = new Event;
	head_->prev = NULL;
	head_->next = tail_;
	tail_->prev = head_;
	tail_->next = NULL;
	code = new char[EVENT_CODE_SIZE];
	ecode = new char[EVENT_CODE_SIZE];
	event_list = new Event[cache_event_size];
		for (int i = 0; i < cache_event_size; i++) {
			this->free_pos.push_back(event_list + i);
		}
	init_log(log_path, log_file_size, log_backup_index);
	this->connection = new HConnection(thrift_ip.c_str(), thrift_port.c_str());
	if (connection && connection->connect()) {
		log4cpp::Category::getRoot().getInstance(username).info(
				"connected to thrift2 at ip=" + thrift_ip + ",port="
						+ thrift_port);
	} else {
		log4cpp::Category::getRoot().getInstance(username).error(
				"errors occurred when connecting to thrift2 at ip=" + thrift_ip
						+ ",port=" + thrift_port);
	}
}
void HTree::free() {
	if(connection&&connection->isconnect()){
	this->connection->disconnect();
	log4cpp::Category::getRoot().getInstance(username).info(
			"disconnect from thrift2...");
	}
	log4cpp::Category::shutdown();
	delete this->connection;
	delete[] event_list;
	delete[] code;
	delete[] ecode;
	delete this->head_;
	delete this->tail_;
}
void HTree::setTableProperty(const string& hTableName,const string& columnFamily){
	this->table = hTableName;
	this->family = columnFamily;
}
void HTree::setBranchAddress(const char* branch_name) {
	if (!strcmp(branch_name, "TEvtHeader")) {
		//EvtHeader = (TEvtHeader*)object;
		m_mask |= hasEvtHeader;
	}
	if (!strcmp(branch_name, "TDigiEvent")) {
		//	DigiEvent = (TDigiEvent*)object;
		m_mask |= hasDigiEvent;
	}
	if (!strcmp(branch_name, "TDstEvent")) {
		//	DstEvent = (TDstEvent*)object;
		m_mask |= hasDstEvent;
	}
	if (!strcmp(branch_name, "TMcEvent")) {
		//	McEvent = (TMcEvent*)object;
		m_mask |= hasMcEvent;
	}
	if (!strcmp(branch_name, "TTrigEvent")) {
		//	TrigEvent = (TTrigEvent*)object;
		m_mask |= hasTrigEvent;
	}
	if (!strcmp(branch_name, "THltEvent")) {
		//	HltEvent = (THltEvent*)object;
		m_mask |= hasHltEvent;
	}
	if (!strcmp(branch_name, "TEvtRecObject")) {
		//	EvtRecObject = (TEvtRecObject*)object;
		m_mask |= hasEvtRecObject;
	}
}
void HTree::getHbaseValues(const string& key, bool isPrefetch) {
	TScan scan;
	scan.__set_startRow(key);
	vector<TResult> _return;
	if(!(connection&&connection->isconnect())){
		cout<<"there is no connection to thrift"<<endl;
	log4cpp::Category::getRoot().getInstance(username).error(
			"there is no connection to thrift");
	exit(1);
	}
	int32_t id = connection->openScanner(table, scan);
	int32_t prefetch_size = isPrefetch ? numRows : 1;
	connection->getScannerRows(_return, id, prefetch_size);
//	log4cpp::Category::getRoot().getInstance(username).info(
//			"open scanner and scan hbase");
	int32_t size = _return.size();
	if (size == 0) {
		exit(0);
	} else {
		log4cpp::Category::getRoot().getInstance(username).info(
				"fetch data from hbase and get " + toString(size) + " events");
	}
	for (int32_t i = 0; i < size; i++) {
		string key = _return[i].row;
		vector<TColumnValue> vec = _return[i].columnValues;
		string d_len = vec[0].value;
		string len = vec[1].value;
		string buffer = vec[2].value;
		if (!free_pos.empty()) {
			Event* data = free_pos.back();
			free_pos.pop_back();
			data->BossFullEvent = buffer;
			data->d_length = atoi(d_len.c_str());
			data->length = atoi(len.c_str());
			data->key = key;
			_hashmap[key] = data;
			attach(data);
		} else { //not enough room
			Event* data = tail_->prev;
			detach(data);
			_hashmap.erase(data->key);
			data->key = key;
			data->BossFullEvent = buffer;
			data->d_length = atoi(d_len.c_str());
			data->length = atoi(len.c_str());
			attach(data);
			_hashmap[key] = data;
		}
	}
	connection->closeScanner(id);
//	log4cpp::Category::getRoot().getInstance(username).info(
//			"close hbase scanner");
}
void HTree::getEntry(const string& rowKey, bool isPrefetch) {
	Event* event;
	//UInt_t data_size = 0;

	event = this->_hashmap[rowKey];
	if (!event) {
		getHbaseValues(rowKey);
		event = this->_hashmap[rowKey];
	}
	if (!event) {
		log4cpp::Category::getRoot().getInstance(username).info(
				"failed to fetch event " + rowKey);
		exit(0);
	}
	log4cpp::Category::getRoot().getInstance(username).info(
			"succeed to fetch event " + rowKey);
	strcpy(code, (event->BossFullEvent).c_str());
	decode(code, event->d_length, ecode, event->length);
	TBossFullEvent* BossFullEvent = (TBossFullEvent*) deserialize(ecode,
			event->length);

	if (BossFullEvent == NULL) {
		cout << "errors occurred when decoding or deserializing" << endl;
		log4cpp::Category::getRoot().getInstance(username).error(
				"errors occurred when decoding or deserializing");
		exit(1);
	}
	if (m_mask == 0) {
		log4cpp::Category::getRoot().getInstance(username).warn(
				"did't set any branch");
		cout << "====================================================" << endl;
		cout << "EventID:\t\t" << BossFullEvent->getEvtHeader()->getEventId()
				<< endl;
		cout << "Time:\t\t" << BossFullEvent->getEvtHeader()->time() << endl;
		cout << "RunId:\t\t" << BossFullEvent->getEvtHeader()->getRunId()
				<< endl;
		cout << "EventTag:\t\t" << BossFullEvent->getEvtHeader()->getEventTag()
				<< endl;
		cout << "Flag1:\t\t" << BossFullEvent->getEvtHeader()->getFlag1()
				<< endl;
		cout << "Flag2:\t\t" << BossFullEvent->getEvtHeader()->getFlag2()
				<< endl;
		cout << "====================================================" << endl;
	}
	if (m_mask & hasEvtHeader) {
		if (BossFullEvent->getEvtHeader()) {
			this->EvtHeader = BossFullEvent->getEvtHeader();
		} else {
			std::cerr << "ERROR: " << "No TEvtHeader in this entry"
					<< std::endl;
			log4cpp::Category::getRoot().getInstance(username).error(
					"No TEvtHeader in this entry");
		}
	}
	if (m_mask & hasDigiEvent) {
		if (BossFullEvent->getDigiEvent()) {
			this->DigiEvent = BossFullEvent->getDigiEvent();
		} else {
			std::cerr << "ERROR: " << "No TDigiEvent in this entry"
					<< std::endl;
			log4cpp::Category::getRoot().getInstance(username).error(
					"No TDigiEvent in this entry");
		}
	}
	if (m_mask & hasDstEvent) {
		if (BossFullEvent->getDstEvent()) {
			this->DstEvent = BossFullEvent->getDstEvent();
		} else {
			std::cerr << "ERROR: " << "No TDstEvent in this entry" << std::endl;
			log4cpp::Category::getRoot().getInstance(username).error(
					"No TDstEvent in this entry");
		}
	}
	if (m_mask & hasMcEvent) {
		if (BossFullEvent->getMcEvent()) {
			this->McEvent = BossFullEvent->getMcEvent();
		} else {
			std::cerr << "ERROR: " << "No TMcEvent in this entry" << std::endl;
			log4cpp::Category::getRoot().getInstance(username).error(
					"No TMcEvent in this entry");

		}
	}
	if (m_mask & hasTrigEvent) {
		if (BossFullEvent->getTrigEvent()) {
			this->TrigEvent = BossFullEvent->getTrigEvent();
		} else {
			std::cerr << "ERROR: " << "No TTrigEvent in this entry"
					<< std::endl;
			log4cpp::Category::getRoot().getInstance(username).error(
					"No TTrigEvent in this entry");

		}
	}
	if (m_mask & hasHltEvent) {
		if (BossFullEvent->getHltEvent()) {
			this->HltEvent = BossFullEvent->getHltEvent();
		} else {
			std::cerr << "ERROR: " << "No THltEvent in this entry" << std::endl;
			log4cpp::Category::getRoot().getInstance(username).error(
					"No THltEvent in this entry");
		}
	}
	if (m_mask & hasEvtRecObject) {
		if (BossFullEvent->getEvtRecObject()) {
			this->EvtRecObject = BossFullEvent->m_evtRecObject;
		} else {
			std::cerr << "ERROR: " << "No TEvtRecObject in this entry"
					<< std::endl;
			log4cpp::Category::getRoot().getInstance(username).error(
					"No TEvtRecObject in this entry");
		}
	}
}
bool HTree::exist(const string rowkey) {
	TGet get;
	get.__set_row(rowkey);
	return connection->exist(table, get);
}
void HTree::detach(Event* node) {
	node->prev->next = node->next;
	node->next->prev = node->prev;
}
void HTree::attach(Event* node) {
	node->prev = head_;
	node->next = head_->next;
	head_->next = node;
	node->next->prev = node;
}
