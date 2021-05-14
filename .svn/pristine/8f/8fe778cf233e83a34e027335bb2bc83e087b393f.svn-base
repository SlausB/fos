

#ifndef HEAVY_BATCH_H
#define HEAVY_BATCH_H

#include "../../../src/fos.h"

#include <vector>

class HeavyBatch: public fos::Batch
{
public:
	
	//virtual ~HeavyBatch();
	
	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	
	void OnDestruction(fos::BatchTime* batchTime);
	
	std::vector<int> load;
};

#endif//#ifndef HEAVY_BATCH_H

