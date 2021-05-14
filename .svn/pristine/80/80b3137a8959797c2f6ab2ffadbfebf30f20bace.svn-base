

#include "exiting_time_imp.h"

#include "../memdbg_start.h"

ExitingTime_Imp::ExitingTime_Imp(Dispatcher* dispatcher, Client* client): dispatcher(dispatcher), client(client)
{
}

#ifndef FOS_NO_DB

db::ResponseRef ExitingTime_Imp::DBRequest(const char* query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef ExitingTime_Imp::DBRequest(const std::string& query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef ExitingTime_Imp::DBRequest(const boost::format& query)
{
	return dispatcher->DBRequest(query);
}

void ExitingTime_Imp::DBRequestA(const char* query)
{
	dispatcher->DBRequestA(query);
}

void ExitingTime_Imp::DBRequestA(const std::string& query)
{
	dispatcher->DBRequestA(query);
}

void ExitingTime_Imp::DBRequestA(const boost::format& query)
{
	dispatcher->DBRequestA(query);
}

#endif//#ifndef FOS_NO_DB

void ExitingTime_Imp::Send(const Outcoming& outcoming)
{
	dispatcher->Send(outcoming);
}

void ExitingTime_Imp::Send(const uint64_t& sessionId, const OutStream& outStream)
{
	dispatcher->Send(sessionId, outStream);
}

void ExitingTime_Imp::Send(const std::vector<uint64_t>& ids, const OutStream& outStream)
{
	dispatcher->Send(ids, outStream);
}

BatchRef ExitingTime_Imp::GetBatch(const BatchId* batchId)
{
	return dispatcher->GetBatch(client, batchId);
}

BatchRef ExitingTime_Imp::GetBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef ExitingTime_Imp::GetBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef ExitingTime_Imp::GetBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	return dispatcher->GetBatch(client, &integralBatchId);
}


void ExitingTime_Imp::DtopBatch(const BatchId* batchId)
{
	dispatcher->DropBatch(client, batchId);
}

void ExitingTime_Imp::DtopBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void ExitingTime_Imp::DtopBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void ExitingTime_Imp::DtopBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	dispatcher->DropBatch(client, &integralBatchId);
}

void ExitingTime_Imp::Disconnect(const uint64_t& id)
{
	dispatcher->Disconnect(id);
}

void ExitingTime_Imp::Disconnect(const std::vector<uint64_t>& ids)
{
	dispatcher->Disconnect(ids);
}

boost::shared_ptr<std::vector<uint64_t> > ExitingTime_Imp::GetIds()
{
	return dispatcher->GetIds();
}

#include "../memdbg_end.h"

