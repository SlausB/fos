

#include "common_time_imp.h"

#include "../memdbg_start.h"

CommonTime_Imp::CommonTime_Imp(Dispatcher* dispatcher, Client* client): dispatcher(dispatcher), client(client)
{
}

#ifndef FOS_NO_DB

db::ResponseRef CommonTime_Imp::DBRequest(const char* query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef CommonTime_Imp::DBRequest(const std::string& query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef CommonTime_Imp::DBRequest(const boost::format& query)
{
	return dispatcher->DBRequest(query);
}

void CommonTime_Imp::DBRequestA(const char* query)
{
	dispatcher->DBRequestA(query);
}

void CommonTime_Imp::DBRequestA(const std::string& query)
{
	dispatcher->DBRequestA(query);
}

void CommonTime_Imp::DBRequestA(const boost::format& query)
{
	dispatcher->DBRequestA(query);
}

std::string CommonTime_Imp::GetDBStats()
{
	return dispatcher->GetDBStats();
}

#endif//#ifndef FOS_NO_DB

size_t CommonTime_Imp::GetCurrentConnectionsCount()
{
	return dispatcher->GetCurrentConnectionsCount();
}

void CommonTime_Imp::Send(const Outcoming& outcoming)
{
	dispatcher->Send(outcoming);
}

void CommonTime_Imp::Send(const uint64_t& sessionId, const OutStream& outStream)
{
	dispatcher->Send(sessionId, outStream);
}

void CommonTime_Imp::Send(const std::vector<uint64_t>& ids, const OutStream& outStream)
{
	dispatcher->Send(ids, outStream);
}

BatchRef CommonTime_Imp::GetBatch(const BatchId* batchId)
{
	return dispatcher->GetBatch(client, batchId);
}

BatchRef CommonTime_Imp::GetBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef CommonTime_Imp::GetBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef CommonTime_Imp::GetBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	return dispatcher->GetBatch(client, &integralBatchId);
}


void CommonTime_Imp::DropBatch(const BatchId* batchId)
{
	dispatcher->DropBatch(client, batchId);
}

void CommonTime_Imp::DropBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void CommonTime_Imp::DropBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void CommonTime_Imp::DropBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	dispatcher->DropBatch(client, &integralBatchId);
}

void CommonTime_Imp::Disconnect(const uint64_t& id)
{
	dispatcher->Disconnect(id);
}

void CommonTime_Imp::Disconnect(const std::vector<uint64_t>& ids)
{
	dispatcher->Disconnect(ids);
}

boost::shared_ptr<std::vector<uint64_t> > CommonTime_Imp::GetIds()
{
	return dispatcher->GetIds();
}

CommonTime* CommonTime_Imp::AllocateCommonTime()
{
	return dispatcher->AllocateCommonTime();
}

#include "../memdbg_end.h"

