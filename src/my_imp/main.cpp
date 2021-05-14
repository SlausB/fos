

#include "fos/src/fos.h"

#include "my_meters_factory.h"
#include "handlers.h"
#include "my_batch_factory.h"


int main()
{
	fos::Run("fos", "progamer", "7777777", new MyMetersFactory, new MyBatchFactory, ::StartingHandler, ::TickHandler, 0.020f, ::ConnectionHandler, ::MessageHandler, ::DisconnectionHandler, ::ExitingHandler, 4646, NULL);
	
	getchar();
	
	return 0;
}

