

#include "luggage_batch.h"

using namespace fos;


LuggageBatch::LuggageBatch()
{
	//Globals::messenger->write("I: LuggageBatch::LuggageBatch()\n");
}

void LuggageBatch::OnCreation(const BatchId* batchId, BatchTime* batchTime)
{
	//Globals::messenger->write("I: LuggageBatch::OnCreation()\n");

	isLoggedIn = false;
	login = "undefined";
}

void LuggageBatch::OnDestruction(BatchTime* batchTime)
{
	//Globals::messenger->write("I: LuggageBatch::OnDestruction()\n");
}

