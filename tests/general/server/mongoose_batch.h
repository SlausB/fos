

#ifndef MONGOOSE_BATCH_H
#define MONGOOSE_BATCH_H

#include "../../../src/fos.h"

#include <vector>

#include "mongoose.h"

class MongooseBatch: public fos::Batch
{
public:
	
	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	
	void OnDestruction(fos::BatchTime* batchTime);
	
	/** Созданный в OnCreation() HTTP сервер.*/
	mg_context* mgContext;
	
	fos::CommonTime* commonTime;
};

#endif//#ifndef MONGOOSE_BATCH_H

