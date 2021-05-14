
#ifndef FACTORY_H
#define FACTORY_H

#include "../../../src/fos.h"

class BatchFactory: public fos::BatchFactory
{
public:
	
	fos::Batch* CreateCustomBatch(const fos::BatchId* batchId);
	
	fos::Batch* CreateLuggageBatch(const uint64_t& sessionId);
};

#endif//#ifndef FACTORY_H

