
#include "factory.h"

#include "luggage_batch.h"
#include "general_batch.h"
#include "heavy_batch.h"
#include "mongoose_batch.h"

using namespace fos;

Batch* ::BatchFactory::CreateCustomBatch(const BatchId* batchId)
{
	//Globals::messenger->write(boost::format("I: BatchFactory::CreateCustomBatch(): %s | %s\n") % batchId->GetType() % batchId->GetId());
	if(batchId->GetType() == BatchId::LITERAL)
	{
		LiteralBatchId* literalBatchId = (LiteralBatchId*)batchId;
		if(literalBatchId->GetValue().compare("buildings") == 0)
		{
			return new BuildingsBatch;
		}
		else if(literalBatchId->GetValue().compare("mongoose") == 0)
		{
			return new MongooseBatch;
		}
	}
	//return new GeneralBatch;
	return new HeavyBatch;
}

Batch* ::BatchFactory::CreateLuggageBatch(const uint64_t& sessionId)
{
	//Globals::messenger->write("I: BatchFactory::CreateLuggageBatch()\n");
	return new LuggageBatch;
}

