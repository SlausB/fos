

#ifndef FOS_CLIENT_TIME_IMP_H
#define FOS_CLIENT_TIME_IMP_H

#include "client_time.h"

namespace fos
{

class Dispatcher;
class Client;

class ClientTime_Imp: public ClientTime
{
public:
	
	ClientTime_Imp(Dispatcher* dispatcher, Client* client);
	
#ifndef FOS_NO_DB
	db::ResponseRef DBRequest( char* query, const bool use, uint32_t size );
	db::ResponseRef DBRequest( const char* query );
	db::ResponseRef DBRequest( const std::string& query );
	db::ResponseRef DBRequest( const boost::format& query );
	void DBRequestA( char* query, const bool use, uint32_t size );
	void DBRequestA( const char* query );
	void DBRequestA( const std::string& query );
	void DBRequestA( const boost::format& query );
#endif//#ifndef FOS_NO_DB
	
	void Send(const Outcoming& outcoming);
	void Send(const OutStream& outStream);
	void Send(const uint64_t& sessionId, const OutStream& outStream);
	void Send(const std::string& data);
	void Send(const void* data, const size_t length);
	
	BatchRef GetBatch(const BatchId* batchId);
	BatchRef GetBatch(const char* id);
	BatchRef GetBatch(const std::string& id);
	BatchRef GetBatch(const uint64_t& id);
	
	void DropBatch(const BatchId* batchId);
	void DropBatch(const char* id);
	void DropBatch(const std::string& id);
	void DropBatch(const uint64_t& id);
	void DropBatch(const BatchId* batchId, const uint64_t sessionId);
	void DropBatch(const char* id, const uint64_t sessionId);
	void DropBatch(const std::string& id, const uint64_t sessionId);
	void DropBatch(const uint64_t& id, const uint64_t sessionId);
	
	void Disconnect(const uint64_t& id);
	void Disconnect(const std::vector<uint64_t>& ids);
	void Disconnect();
	
	boost::shared_ptr<std::vector<uint64_t> > GetIds();
	
	CommonTime* AllocateCommonTime();
	
private:
	
	Dispatcher* dispatcher;
	Client* client;
};

}//namespace fos

#endif//#ifndef FOS_CLIENT_TIME_IMP_H

