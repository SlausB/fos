

#include "heavy_batch.h"

using namespace fos;


/*HeavyBatch::~HeavyBatch()
{
}*/

void HeavyBatch::OnCreation(const BatchId* batchId, BatchTime* batchTime)
{
	for(int i = 0; i < 1000000; i++)
	{
		load.push_back(i);
	}
}

void HeavyBatch::OnDestruction(BatchTime* batchTime)
{
}

