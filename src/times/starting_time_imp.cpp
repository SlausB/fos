

#include "starting_time_imp.h"

#include "../memdbg_start.h"

namespace fos
{

StartingTime_Imp::StartingTime_Imp(Dispatcher* dispatcher, Client* client): dispatcher(dispatcher), client(client)
{
}

#ifndef FOS_NO_DB

db::ResponseRef StartingTime_Imp::DBRequest(const char* query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef StartingTime_Imp::DBRequest(const std::string& query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef StartingTime_Imp::DBRequest(const boost::format& query)
{
	return dispatcher->DBRequest(query);
}

void StartingTime_Imp::DBRequestA(const char* query)
{
	dispatcher->DBRequestA(query);
}

void StartingTime_Imp::DBRequestA(const std::string& query)
{
	dispatcher->DBRequestA(query);
}

void StartingTime_Imp::DBRequestA(const boost::format& query)
{
	dispatcher->DBRequestA(query);
}

void StartingTime_Imp::DBAddFilter(const char* msg)
{
	dispatcher->DBAddFilter(msg);
}

#endif//#ifndef FOS_NO_DB

BatchRef StartingTime_Imp::GetBatch(const BatchId* batchId)
{
	return dispatcher->GetBatch(client, batchId);
}

BatchRef StartingTime_Imp::GetBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef StartingTime_Imp::GetBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	return dispatcher->GetBatch(client, &literalBatchId);
}

BatchRef StartingTime_Imp::GetBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	return dispatcher->GetBatch(client, &integralBatchId);
}


void StartingTime_Imp::DropBatch(const BatchId* batchId)
{
	dispatcher->DropBatch(client, batchId);
}

void StartingTime_Imp::DropBatch(const char* id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void StartingTime_Imp::DropBatch(const std::string& id)
{
	LiteralBatchId literalBatchId(id);
	dispatcher->DropBatch(client, &literalBatchId);
}

void StartingTime_Imp::DropBatch(const uint64_t& id)
{
	IntegralBatchId integralBatchId(id);
	dispatcher->DropBatch(client, &integralBatchId);
}

CommonTime* StartingTime_Imp::AllocateCommonTime()
{
	return dispatcher->AllocateCommonTime();
}

Dispatcher* StartingTime_Imp::GetDispatcher()
{
	return dispatcher;
}

}//namespace fos

#include "../memdbg_end.h"

