

#ifndef PLAYER_BATCH_H
#define PLAYER_BATCH_H

#include "../../../src/fos.h"

class Building
{
public:
	
	int id;
	
	int type;
	
	int x;
	int y;
	
	time_t lastUpdate;
};

class PlayerBatch: public fos::Batch
{
public:
	
	void OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime);
	void OnDesctruction(fos::BatchTime* batchTime);
	
	
	std::string townName;
	int coins;
	
	std::vector<Building*> buildings;
};

#endif//#ifndef PLAYER_BATCH_H

