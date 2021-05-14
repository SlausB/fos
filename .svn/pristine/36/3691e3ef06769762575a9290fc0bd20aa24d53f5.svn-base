

#include "tick_time_imp.h"

#include "../memdbg_start.h"

TickTime_Imp::TickTime_Imp(Dispatcher* dispatcher, Client* client): dispatcher(dispatcher), client(client)
{
}

#ifndef FOS_NO_DB

db::ResponseRef TickTime_Imp::DBRequest(const char* query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef TickTime_Imp::DBRequest(const std::string& query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef TickTime_Imp::DBRequest(const boost::format& query)
{
	return dispatcher->DBRequest(query);
}

void TickTime_Imp::DBRequestA(const char* query)
{
	dispatcher->DBRequestA(query);
}

void TickTime_Imp::DBRequestA(const std::string& query)
{
	dispatcher->DBRequestA(query);
}

void TickTime_Imp::DBRequestA(const boost::format& query)
{
	dispatcher->DBRequestA(query);
}

#endif//#ifndef FOS_NO_DB

void TickTime_Imp::Send(const Outcoming& outcoming)
{
	dispatcher->Send(outcoming);
}

void TickTime_Imp::Send(const uint64_t& sessionId, const OutStream& outStream)
{
	dispatcher->Send(sessionId, outStream);
}

void TickTime_Imp::Send(const std::vector<uint64_t>& ids, const OutStream& outStream)
{
	dispatcher->Send(ids, outStream);
}

BatchRef TickTime_Imp::GetBatch(const BatchId* batchId)
{
	return dispatcher->GetBatch(client, batchId);
}

BatchRef TickTime_Imp::GetBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef TickTime_Imp::GetBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef TickTime_Imp::GetBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	return dispatcher->GetBatch(client, &integralBatchId);
}


void TickTime_Imp::DropBatch(const BatchId* batchId)
{
	dispatcher->DropBatch(client, batchId);
}

void TickTime_Imp::DropBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void TickTime_Imp::DropBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void TickTime_Imp::DropBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	dispatcher->DropBatch(client, &integralBatchId);
}

CommonTime* TickTime_Imp::AllocateCommonTime()
{
	return dispatcher->AllocateCommonTime();
}

Dispatcher* TickTime_Imp::GetDispatcher()
{
	return dispatcher;
}

void TickTime_Imp::Disconnect( const uint64_t& id )
{
	dispatcher->Disconnect( id );
}

void TickTime_Imp::Disconnect( const std::vector<uint64_t>& ids )
{
	dispatcher->Disconnect( ids );
}

#include "../memdbg_end.h"

