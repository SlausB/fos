
#ifndef FOS_TICK_TIME_IMP_H
#define FOS_TICK_TIME_IMP_H

#include "tick_time.h"

namespace fos
{

class Dispatcher;
class Client;
class CommonTime;

/** Интерфейс с сервером при обработке события таймера.*/
class TickTime_Imp: public TickTime
{
public:
	
	TickTime_Imp(Dispatcher* dispatcher, Client* client);
	
#ifndef FOS_NO_DB
	db::ResponseRef DBRequest(const char* query);
	db::ResponseRef DBRequest(const std::string& query);
	db::ResponseRef DBRequest(const boost::format& query);
	void DBRequestA(const char* query);
	void DBRequestA(const std::string& query);
	void DBRequestA(const boost::format& query);
#endif//#ifndef FOS_NO_DB
	
	void Send(const Outcoming& outcoming);
	void Send(const uint64_t& sessionId, const OutStream& outStream);
	void Send(const std::vector<uint64_t>& ids, const OutStream& outStream);
	
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
	
	void Disconnect(const uint64_t& id);
	void Disconnect(const std::vector<uint64_t>& ids);
	
private:
	
	Dispatcher* dispatcher;
	Client* client;
};

}//namespace fos

#endif//#ifndef FOS_TICK_TIME_IMP_H

