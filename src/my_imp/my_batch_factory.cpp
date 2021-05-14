

#include "my_batch_factory.h"

#include "luggage_batch.h"
#include "common_batch.h"


fos::Batch* MyBatchFactory::CreateCustomBatch(const fos::BatchId* batchId)
{
	return new CommonBatch;
}

fos::Batch* MyBatchFactory::CreateLuggageBatch(const uint64_t& sessionId)
{
	return new LuggageBatch;
}

