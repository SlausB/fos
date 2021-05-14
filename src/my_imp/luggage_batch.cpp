

#include "luggage_batch.h"


LuggageBatch::LuggageBatch()
{
}

void LuggageBatch::OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime)
{
}

void LuggageBatch::OnDestruction(fos::BatchTime* batchTime)
{
}

std::string LuggageBatch::Describe()
{
	return "describe shortly this luggage batch here";
}

