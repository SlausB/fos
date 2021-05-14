
#ifndef FOS_STARTING_TIME_IMP_H
#define FOS_STARTING_TIME_IMP_H

#include "starting_time.h"

namespace fos
{

class Dispatcher;
class Client;
class CommonTime;

class StartingTime_Imp: public StartingTime
{
public:
	
	StartingTime_Imp(Dispatcher* dispatcher, Client* client);
	
#ifndef FOS_NO_DB
	db::ResponseRef DBRequest(const char* query);
	db::ResponseRef DBRequest(const std::string& query);
	db::ResponseRef DBRequest(const boost::format& query);
	void DBRequestA(const char* query);
	void DBRequestA(const std::string& query);
	void DBRequestA(const boost::format& query);
	void DBAddFilter(const char* msg);
#endif//#ifndef FOS_NO_DB
	
	BatchRef GetBatch(const BatchId* batchId);
	BatchRef GetBatch(const char* id);
	BatchRef GetBatch(const std::string& id);
	BatchRef GetBatch(const uint64_t& id);
	
	void DropBatch(const BatchId* batchId);
	void DropBatch(const char* id);
	void DropBatch(const std::string& id);
	void DropBatch(const uint64_t& id);
	
	CommonTime* AllocateCommonTime();
	
	Dispatcher* GetDispatcher();
	
private:
	
	Dispatcher* dispatcher;
	Client* client;
};

}//namespace fos

#endif//#ifndef FOS_STARTING_TIME_IMP_H

