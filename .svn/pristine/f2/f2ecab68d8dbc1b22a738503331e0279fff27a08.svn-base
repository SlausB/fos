

#include "handlers.h"

#include "mongoose_batch.h"

using namespace fos;

void StartingHandler(StartingTime* startingTime, void*)
{
	db::ResponseRef myResponse = startingTime->DBRequest("SELECT something FROM everywhere WHERE everything IS anything");

	const bool isOk = myResponse.IsOk();
	const int resultsCount = myResponse.GetResultsCount();
	if(isOk && resultsCount != 1)
	{
		Globals::messenger->write(boost::format("E: StartingHandler(): isOk = %d, resultsCount = %d instead of %d | %d.\n") % isOk % resultsCount);
	}

	startingTime->DBRequest("DROP TABLE gentest_players");
	startingTime->DBRequest(
		"CREATE TABLE gentest_players (\
		last_session_id			bigint,\
		login					text,\
		password				text,\
		last_appearance			date\
		);");
	startingTime->DBRequest("DELETE FROM gentest_players");


	/*BatchRef batchRef = startingTime->GetBatch("general");
	for(int i = 0; i < 1000; i++)
	{
		std::string name = str(boost::format("general_%d") % i);
		Globals::messenger->write(boost::format("I: requesting \"%s\".\n") % name);
		BatchRef batchRef = startingTime->GetBatch(name);
	}*/
	
	
	
	//запускаем HTTP сервер:
	startingTime->GetBatch("mongoose");
}

