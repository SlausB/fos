

#include "run.h"

#include "dispatcher.h"
#include "colonel/colonel.h"
#include "colonel_imp/handlers/exit_h.h"
#include "colonel_imp/handlers/dbstats_h.h"
#include "colonel_imp/handlers/conns_h.h"
#include "colonel_imp/handlers/clients_h.h"
#include "colonel_imp/handlers/seg_fault_h.h"
#include "colonel_imp/requesters/con_req.h"

#include "memdbg_start.h"


namespace fos
{

#ifdef FOS_NO_DB

void Run(MetersFactory* metersFactory, 
		BatchFactory* batchFactory, 
		StartingHandler startingHandler, 
		TickHandler tickHandler, 
		const double tickPeriod, 
		ConnectionHandler connectionHandler, 
		MessageHandler messageHandler, 
		DisconnectionHandler disconnectionHandler, 
		ExitingHandler exitingHandler, 
		int clientsPort,
		void* clientData)
{
	setlocale(LC_CTYPE, "");
	
	{
		Dispatcher dispatcher(metersFactory, 
							batchFactory, 
							startingHandler, 
							tickHandler, 
							tickPeriod, 
							connectionHandler, 
							messageHandler, 
							disconnectionHandler, 
							exitingHandler, 
							clientsPort, 
							clientData);
		
		ContextLock* contextLock = new ContextLock;
		
		
		{
			Colonel colonel;
			
			Exit_Handler exit_Handler;
			colonel.PushHandler(&exit_Handler);
			
			Conns_Handler conns_Handler( dispatcher.AllocateCommonTime() );
			colonel.PushHandler(&conns_Handler);
			
			Clients_Handler clients_Handler(&dispatcher);
			colonel.PushHandler(&clients_Handler);
			
			SegFault_Handler segFault_Handler;
			colonel.PushHandler( &segFault_Handler );
			
			ConsoleRequester consoleRequester(&colonel, Globals::messenger, contextLock);
			
			contextLock->Stop();
		}
	}

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)
	_CrtDumpMemoryLeaks();
#endif
}

#else

void Run(const char* dbName, 
		const char* dbLogin, 
		const char* dbPassword, 
		MetersFactory* metersFactory, 
		BatchFactory* batchFactory, 
		StartingHandler startingHandler, 
		TickHandler tickHandler, 
		const double tickPeriod, 
		ConnectionHandler connectionHandler, 
		MessageHandler messageHandler, 
		DisconnectionHandler disconnectionHandler, 
		ExitingHandler exitingHandler, 
		int clientsPort,
		void* clientData)
{
	setlocale(LC_CTYPE, "");
	
	{
		Dispatcher dispatcher(dbName, 
							dbLogin, 
							dbPassword, 
							metersFactory, 
							batchFactory, 
							startingHandler, 
							tickHandler, 
							tickPeriod, 
							connectionHandler, 
							messageHandler, 
							disconnectionHandler, 
							exitingHandler, 
							clientsPort, 
							clientData);
		
		ContextLock* contextLock = new ContextLock;
		
		
		{
			Colonel colonel;
			
			Exit_Handler exit_Handler;
			colonel.PushHandler(&exit_Handler);
			
			DBStats_Handler dbStats_Handler(dispatcher.AllocateCommonTime());
			colonel.PushHandler(&dbStats_Handler);
			
			Conns_Handler conns_Handler(dispatcher.AllocateCommonTime());
			colonel.PushHandler(&conns_Handler);
			
			Clients_Handler clients_Handler(&dispatcher);
			colonel.PushHandler(&clients_Handler);
			
			SegFault_Handler segFault_Handler;
			colonel.PushHandler( &segFault_Handler );
			
			ConsoleRequester consoleRequester(&colonel, Globals::messenger, contextLock);
			
			contextLock->Stop();
		}
	}

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)
	_CrtDumpMemoryLeaks();
#endif
}

#endif//#ifdef FOS_NO_DB

}//namespace fos

#include "memdbg_end.h"

