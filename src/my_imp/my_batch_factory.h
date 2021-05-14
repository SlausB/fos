

#ifndef MY_BATCH_FACTORY_H
#define MY_BATCH_FACTORY_H

#include "fos/src/fos.h"

class MyBatchFactory: public fos::BatchFactory
{
public:
	
	fos::Batch* CreateCustomBatch(const fos::BatchId* batchId);
	
	fos::Batch* CreateLuggageBatch(const uint64_t& sessionId);
};

#endif//#ifndef MY_BATCH_FACTORY_H

