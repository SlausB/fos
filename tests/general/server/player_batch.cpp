

#include "player_batch.h"

using namespace fos;

void PlayerBatch::OnCreation(const BatchId* batchId, BatchTime* batchTime)
{
	LiteralBatchId* playerBatchId = (LiteralBatchId*)batchId;
	const std::string& login = playerBatchId->GetValue();
	
	db::ResponseRef playerNameDBData = batchTime->DBRequest(str(boost::format("INSERT INTO players (name, coins) VALUES (%s, 10000); SELECT id FROM players WHERE name = '%s';") % login % login));
	
	const uint64_t playerId = playerNameDBData.GetResult(1)->GetInt64(0, "id");
	Globals::messenger->write(boost::format("I: PlayerBatch::OnCreation(): got playerId = %lld.\n") % playerId);
	
	db::ResponseRef playerDBData = batchTime->DBRequest(str(boost::format("SELECT id, type, x, y FROM buildings WHERE player_id = %lld;") % playerId));
}

void PlayerBatch::OnDesctruction(fos::BatchTime* batchTime)
{
	Globals::messenger->write(boost::format("I: PlayerBatch::OnDesctruction()\n"));
}

