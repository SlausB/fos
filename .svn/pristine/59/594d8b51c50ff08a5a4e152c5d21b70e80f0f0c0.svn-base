

#ifndef COMMON_BATCH_H
#define COMMON_BATCH_H

#include "fos/src/fos.h"

class CommonBatch: public fos::Batch
{
public:
	CommonBatch();
	
	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	
	void OnDestruction(fos::BatchTime* batchTime);
	
	std::string Describe();
};

#endif//#ifndef COMMON_BATCH_H

