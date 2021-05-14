

#include "client_time_imp.h"

#include "../memdbg_start.h"

ClientTime_Imp::ClientTime_Imp(Dispatcher* dispatcher, Client* client): dispatcher(dispatcher), client(client)
{
}

#ifndef FOS_NO_DB

db::ResponseRef ClientTime_Imp::DBRequest( char* query, const bool use, uint32_t size )
{
	return dispatcher->DBRequest( query, use, size );
}

db::ResponseRef ClientTime_Imp::DBRequest( const char* query )
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef ClientTime_Imp::DBRequest(const std::string& query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef ClientTime_Imp::DBRequest(const boost::format& query)
{
	return dispatcher->DBRequest(query);
}

void ClientTime_Imp::DBRequestA( char* query, const bool use, uint32_t size )
{
	dispatcher->DBRequestA( query, use, size );
}

void ClientTime_Imp::DBRequestA(const char* query)
{
	dispatcher->DBRequestA(query);
}

void ClientTime_Imp::DBRequestA(const std::string& query)
{
	dispatcher->DBRequestA(query);
}

void ClientTime_Imp::DBRequestA(const boost::format& query)
{
	dispatcher->DBRequestA(query);
}

#endif//#ifndef FOS_NO_DB

void ClientTime_Imp::Send(const Outcoming& outcoming)
{
	dispatcher->Send(outcoming);
}

void ClientTime_Imp::Send(const OutStream& outStream)
{
	dispatcher->Send(client->sessionId, outStream);
}

void ClientTime_Imp::Send(const uint64_t& sessionId, const OutStream& outStream)
{
	dispatcher->Send(sessionId, outStream);
}

void ClientTime_Imp::Send(const std::string& data)
{
	dispatcher->Send(client->sessionId, data);
}

void ClientTime_Imp::Send(const void* data, const size_t length)
{
	dispatcher->Send(client->sessionId, data, length);
}

BatchRef ClientTime_Imp::GetBatch(const BatchId* batchId)
{
	return dispatcher->GetBatch(client, batchId);
}

BatchRef ClientTime_Imp::GetBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef ClientTime_Imp::GetBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef ClientTime_Imp::GetBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	return dispatcher->GetBatch(client, &integralBatchId);
}


void ClientTime_Imp::DropBatch(const BatchId* batchId)
{
	dispatcher->DropBatch(client, batchId);
}

void ClientTime_Imp::DropBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void ClientTime_Imp::DropBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void ClientTime_Imp::DropBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	dispatcher->DropBatch(client, &integralBatchId);
}

void ClientTime_Imp::DropBatch(const BatchId* batchId, const uint64_t sessionId)
{
	dispatcher->DropBatch(sessionId, batchId);
}

void ClientTime_Imp::DropBatch(const char* id, const uint64_t sessionId)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(sessionId, &literalBatchId);
}

void ClientTime_Imp::DropBatch(const std::string& id, const uint64_t sessionId)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(sessionId, &literalBatchId);
}

void ClientTime_Imp::DropBatch(const uint64_t& id, const uint64_t sessionId)
{
	IntegralBatchId integralBatchId(id);
	dispatcher->DropBatch(sessionId, &integralBatchId);
}

void ClientTime_Imp::Disconnect(const uint64_t& id)
{
	dispatcher->Disconnect(id);
}

void ClientTime_Imp::Disconnect(const std::vector<uint64_t>& ids)
{
	dispatcher->Disconnect(ids);
}

void ClientTime_Imp::Disconnect()
{
	dispatcher->Disconnect(client->sessionId);
}
	
boost::shared_ptr<std::vector<uint64_t> > ClientTime_Imp::GetIds()
{
	return dispatcher->GetIds();
}

CommonTime* ClientTime_Imp::AllocateCommonTime()
{
	return dispatcher->AllocateCommonTime();
}

#include "../memdbg_end.h"

