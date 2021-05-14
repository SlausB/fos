
#ifndef LUGGAGE_BATCH_H
#define LUGGAGE_BATCH_H

#include "../../../src/fos.h"

class LuggageBatch: public fos::Batch
{
public:
	
	LuggageBatch();

	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	void OnDestruction(fos::BatchTime* batchTime);


	bool isLoggedIn;
	std::string login;
};

#endif//#ifndef LUGGAGE_BATCH_H

