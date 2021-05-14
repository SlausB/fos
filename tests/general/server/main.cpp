
#include "../../../src/fos.h"

#include "factory.h"
#include "handlers.h"

int main()
{
	BatchFactory* batchFactory = new BatchFactory;
	
	fos::Run("fos", 
			"slav", 
			"7777777", 
			batchFactory, 
			StartingHandler, 
			TickHandler, 
			0.020f, 
			ConnectionHandler, 
			MessageHandler, 
			DisconnectionHandler, 
			ExitingHandler, 
			4570,
			NULL);
	
	delete batchFactory;
	
	return 0;
}

