

#ifndef LUGGAGE_BATCH_H
#define LUGGAGE_BATCH_H

#include "fos/src/fos.h"

class LuggageBatch: public fos::Batch
{
public:
	LuggageBatch();
	
	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	
	void OnDestruction(fos::BatchTime* batchTime);
	
	std::string Describe();
};

#endif//#ifndef LUGGAGE_BATCH_H

