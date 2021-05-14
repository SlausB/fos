

#ifndef GENERAL_BATCH_H
#define GENERAL_BATCH_H

#include "../../../src/fos.h"

#include <vector>

class GeneralBatch: public fos::Batch
{
public:
	
	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	
	void OnDestruction(fos::BatchTime* batchTime);
};

class BuildingsBatch: public fos::Batch
{
public:
	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	void OnDestruction(fos::BatchTime* batchTime);

	std::vector<int32_t> types;
};

#endif//#ifndef GENERAL_BATCH_H

