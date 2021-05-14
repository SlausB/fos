

#include "dispatcher.h"

#include "externs.h"

#include "clients_conn_iface.h"

#include <boost/asio.hpp>

#include "handlers/clients_handler.h"

#include "batch_factory.h"

#include "times/starting_time_imp.h"

#include "times/tick_time_imp.h"

#include "times/exiting_time_imp.h"

#include "timer.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "memdbg_start.h"

#include "boost/interprocess/sync/scoped_lock.hpp"

#include <exception>

	
#ifdef FOS_HANDLER_TYPE
	#if (FOS_HANDLER_TYPE == FOS_HANDLER_REALTIME)
	#include "handlers/clients_handler_realtime.h"
	#elif (FOS_HANDLER_TYPE == FOS_HANDLER_AIO)
	#include "handlers/aio/clients_handler_aio.h"
	#else
		#define FOS_DEFAULT_HANDLER
	#endif
#else//#ifdef FOS_HANDLER_TYPE
#define FOS_DEFAULT_HANDLER
#endif//#ifdef FOS_HANDLER_TYPE
#ifdef FOS_DEFAULT_HANDLER
	#include "handlers/asio/clients_handler_asio.h"
	#undef FOS_DEFAULT_HANDLER
#endif//#ifdef FOS_DEFAULT_HANDLER


namespace fos
{

Dispatcher::Dispatcher(
#ifndef FOS_NO_DB
					const char* dbName, 
					const char* dbLogin, 
					const char* dbPassword, 
#endif
					MetersFactory* metersFactory, 
					BatchFactory* batchFactory, 
					StartingHandler startingHandler, 
					TickHandler tickHandler, 
					const double tickPeriod, 
					ConnectionHandler connectionHandler, 
					MessageHandler messageHandler, 
					DisconnectionHandler disconnectionHandler, 
					ExitingHandler exitingHandler, 
					const int clientsPort, 
					void* clientData):
	batchFactory(batchFactory), 
	messageHandler(messageHandler), 
	connectionHandler(connectionHandler), 
	disconnectionHandler(disconnectionHandler), 
	exitingHandler(exitingHandler), 
	handlersPool(50), 
	immortalClient(Client::IMMORTAL, 1, false, NULL, "n/a"), 
	lastSessionId(1), 
	stoppingServer(false), 
	tickRunnerExited(false),
	batchesCount(0)
{
#ifndef FOS_NO_DB
	//подключаемся к базе данных:
	pqlDbConn = new pq_db::PqlDbConn(Globals::messenger);
	pqlDbConn->Connect(dbName, dbLogin, dbPassword);
#endif
	
	Construct(metersFactory, startingHandler, tickHandler, tickPeriod, clientsPort, clientData);
}



/** Way to find exception source (root - place from which it was thrown).*/
#if defined(LINUX)

#include <execinfo.h>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <exception>

void my_terminate()
{
	void * array[50];
	int size = backtrace(array, 50);
	
	std::cerr << __FUNCTION__ << " backtrace returned " << size << " frames\n\n";
	
	char ** messages = backtrace_symbols(array, size);
	
	for (int i = 0; i < size && messages != NULL; ++i)
	{
		std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
	}
	std::cerr << std::endl;
	
	free(messages);
	
	//set debugging to this (next) line to watch callstack on unhandled exception:
	abort();
}

//#if defined(LINUX)
#else

void my_terminate()
{
	//set debugging to this (next) line to watch callstack on unhandled exception:
	abort();
}

#endif//#if defined(LINUX)



void Dispatcher::Construct( MetersFactory* metersFactory, StartingHandler startingHandler, TickHandler tickHandler, const double tickPeriod, const int clientsPort, void* clientData )
{
	std::set_terminate( my_terminate );
	
	clientsConnIFace = new ClientsConnIFace( this );
	
	batchTime_Imp = new BatchTime_Imp( this );
	
	//вызываем StartingHandler:
	StartingTime_Imp startingTime_Imp( this, &immortalClient );
	startingHandler( &startingTime_Imp, clientData );
	
#ifdef FOS_HANDLER_TYPE
	#if (FOS_HANDLER_TYPE == FOS_HANDLER_REALTIME)
	Globals::messenger->write(boost::format("I: Dispatcher::Construct(): choosing \"Realtime\" as clients handler.\n"));
	clientsHandler = new ClientsHandler_Realtime(clientsPort, clientsConnIFace);
	#elif (FOS_HANDLER_TYPE == FOS_HANDLER_AIO)
	Globals::messenger->write(boost::format("I: Dispatcher::Construct(): choosing \"Aio\" as clients handler.\n"));
	clientsHandler = new ClientsHandler_Aio(clientsPort, clientsConnIFace);
	#else
		#define FOS_DEFAULT_HANDLER
	#endif
#else//#ifdef FOS_HANDLER_TYPE
#define FOS_DEFAULT_HANDLER
#endif//#ifdef FOS_HANDLER_TYPE
#ifdef FOS_DEFAULT_HANDLER
	Globals::messenger->write(boost::format("I: Dispatcher::Construct(): choosing \"Asio\" as clients handler.\n"));
	clientsHandler = new ClientsHandler_Asio(metersFactory, clientsPort, clientsConnIFace);
	#undef FOS_DEFAULT_HANDLER
#endif//#ifdef FOS_DEFAULT_HANDLER
	
	boost::thread tickThread(&Dispatcher::TickRunner, this, tickHandler, tickPeriod);
}


Dispatcher::~Dispatcher()
{
	Globals::messenger->write(boost::format("I: Dispathcer::~Dispatcher(): exiting...\n"));
	
	stoppingServer = true;
	
	//останавливаем поток вызова функции по таймеру:
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): waiting for tick runner to stop...\n"));
	while(!tickRunnerExited)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): tick runner successfully stopped.\n"));
	
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): cancelling accepting connections...\n"));
	clientsConnIFace->CancelAcceptingConnections();
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): accepting connections canceled.\n"));
	
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispathcer(): calling ExitingHandler()...\n"));
	{
		ExitingTime_Imp exitingTime_Imp(this, &immortalClient);
		exitingHandler(&exitingTime_Imp);
	}
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispathcer(): ExitingHandler() called.\n"));
	
	clientsConnIFace->DisconnectClients();
	
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispathcer(): waiting for all clients to handle all events...\n"));
	for(;;)
	{
		clientsMutex.lock();
		const bool empty = clients.empty();
		clientsMutex.unlock();
		if(empty)
		{
			break;
		}
		else
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): all events successfully handled.\n"));
	
	delete clientsConnIFace;
	
	Globals::messenger->write(boost::format("I: Dispathcer::~Dispatcher(): removing immortal client...\n"));
	ReleaseBatches(&immortalClient);
	immortalClient.batchesUnderUsage.batches.clear();
	Globals::messenger->write(boost::format("I: Dispathcer::~Dispatcher(): immortal client successfully removed.\n"));
	
	if(!clients.empty())
	{
		Globals::messenger->write(boost::format("E: Dispatcher::~Dispatcher(): left some clients.\n"));
	}
	
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): waiting for all clients threads to end...\n"));
	for(;;)
	{
		batchesMutex.lock();
		const size_t size = batchUsageMap.batches.size();
		batchesMutex.unlock();
		if(size > 0)
		{
			Globals::messenger->write(boost::format("W: Dispatcher::~Dispatcher(): left %u batches:\n") % size);
			batchesMutex.lock();
			for(BatchUsageMap::Iterator it = batchUsageMap.batches.begin(); it != batchUsageMap.batches.end(); it++)
			{
				BatchId* batchId = it->first.batchId;
				switch(batchId->GetType())
				{
				case BatchId::INTEGRAL:
					{
						IntegralBatchId* integralBatchId = (IntegralBatchId*)batchId;
						Globals::messenger->write(boost::format("	INTEGRAL: %llu\n") % integralBatchId->GetValue());
					}
					break;
				case BatchId::LITERAL:
					{
						LiteralBatchId* literalBatchId = (LiteralBatchId*)batchId;
						Globals::messenger->write(boost::format("	LITERAL: %s\n") % literalBatchId->GetValue());
					}
					break;
				}
			}
			batchesMutex.unlock();
			
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
		else
		{
			break;
		}
	}
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): all clients threads successfully ended.\n"));
	
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): waiting for all batches to be destructed...\n"));
	for(;;)
	{
		batchesMutex.lock();
		const int leftBatches = batchesCount;
		batchesMutex.unlock();
		if(leftBatches > 0)
		{
			Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): left %d batches...\n") % leftBatches);
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
		else
		{
			break;
		}
	}
	Globals::messenger->write(boost::format("I: Dispatcher::~Dispatcher(): all batches successfully destructed.\n"));
	
	delete batchTime_Imp;
#ifndef FOS_NO_DB
	delete pqlDbConn;
#endif
	
	Globals::messenger->write(boost::format("I: Dispathcer::~Dispatcher(): exited.\n"));
}

int ToInt(const double value)
{
    if(value >= 0.0f) return (int)(value + 0.5f);
    else return (int)(value - 0.5f);
}

void Dispatcher::TickRunner(TickHandler tickHandler, const double period)
{
	boost::this_thread::sleep(boost::posix_time::milliseconds(ToInt(period * 1000.0f))); 
	
	Timer waitingTimer;
	Timer realTimer;
	
	TickTime_Imp tickTime_Imp(this, &immortalClient);
	
	for( ;; )
	{
		if ( stoppingServer )
			break;
		
		const double real = realTimer.getElapsedSeconds();
		waitingTimer.drop();
		tickHandler( real, &tickTime_Imp );
		
		if ( stoppingServer )
			break;
		
		const double consumed = waitingTimer.getElapsedSeconds();
		if ( consumed < period )
			boost::this_thread::sleep( boost::posix_time::milliseconds( ToInt( ( period - consumed ) * 1000.0f ) ) );
	}
	
	tickRunnerExited = true;
}


/** Batches.*/

/** Используется при удалении ссылки на данные (освобождение данных после их использования).*/

class BatchRefOpaqueData
{
public:
	
	BatchRefOpaqueData( Dispatcher* dispatcher, BatchUsageMap::Usage* usage ): dispatcher( dispatcher ), usage( usage )
	{
	}
	
	/** Чтобы вызвать Dispatcher::ReleaseBatch().*/
	Dispatcher* dispatcher;
	
	BatchUsageMap::Usage* usage;
};

void OnBatchDestruction( void* opaqueData )
{
	BatchRefOpaqueData* batchRefOpaqueData = ( BatchRefOpaqueData* ) opaqueData;
	
	batchRefOpaqueData->dispatcher->ReleaseBatch( batchRefOpaqueData->usage );
	
	delete batchRefOpaqueData;
}

void Dispatcher::ReleaseBatch_ThreadUnsafe( BatchUsageMap::Usage* usage )
{
	//если данные больше никто не использует, то их нужно удалить:
	if ( usage->requesters.empty() )
	{
		//если не зарегистрировано ни одно пользователя эти данных, то освобождаем их:
		if ( usage->users.empty() )
		{
			usage->batch->OnDestruction( batchTime_Imp );
			batchesCount--;
			BatchUsageMap::Key key( usage->batchId, BatchUsageMap::Key::SHARED );
			if ( batchUsageMap.batches.erase( key ) != 1 )
				Globals::messenger->write( "E: Dispatcher::ReleaseBatch_ThreadUnsafe(): usage was NOT erased.\n" );
			delete usage;
		}
		else
		{
			usage->underUsage = false;
		}
	}
	//если кто-то ожидает эти данные:
	else
	{
		usage->requesters.front()->Resume();
		usage->requesters.pop_front();
	}
}

void Dispatcher::ReleaseBatch( BatchUsageMap::Usage* usage )
{
	batchesMutex.lock();

	ReleaseBatch_ThreadUnsafe( usage );
	
	batchesMutex.unlock();
}

void Dispatcher::DropBatch( Client* client, const BatchId* batchId )
{
	//мьютекс нужно использовать только для неординарного клиента - для остальных сообщения обрабатываются поочерёдно:
	if ( client->type == Client::IMMORTAL )
		immortalClientMutex.lock();
	
	ReleaseClientsBatch_TU( client, batchId );
	
	if ( client->type == Client::IMMORTAL )
		immortalClientMutex.unlock();
}

void Dispatcher::DropBatch( const uint64_t sessionId, const BatchId* batchId )
{
	Client::Event* event = new Client::Event( Client::Event::DROP_BATCH );
	event->batchId = batchId->MakeCopy();
	HandleEvent( sessionId, event );
}

void Dispatcher::Disconnect( const uint64_t& id )
{
	clientsConnIFace->Disconnect( id );
}

void Dispatcher::Disconnect(const std::vector<uint64_t>& ids)
{
	clientsConnIFace->Disconnect(ids);
}

boost::shared_ptr<std::vector<uint64_t> > Dispatcher::GetIds()
{
	return clientsConnIFace->GetIds();
}

CommonTime* Dispatcher::AllocateCommonTime()
{
	return new CommonTime_Imp(this, &immortalClient);
}

BatchRef Dispatcher::GetBatch( Client* client, const BatchId* batchId )
{
	//ищем данные с указанным идентификатором:
	batchesMutex.lock();

	BatchUsageMap::Key key( batchId, BatchUsageMap::Key::SHARED );
	BatchUsageMap::Iterator it = batchUsageMap.batches.find( key );
	//данные найдены:
	if ( it != batchUsageMap.batches.end() )
	{
		BatchUsageMap::Usage* usage = it->second.usage;
		
		if ( usage == NULL )
		{
			batchesMutex.unlock();
			Globals::messenger->write( "E: Dispatcher::RequestBatch(): found data is NULL. Do nothing.\n" );
			return BatchRef( NULL, NULL, NULL );
		}
		else
		{
			//указываем что данные используются текущим клиентом (это нормально что использование добавляется без проверки - в итоге будет лишь один экземпляр):
			{
				usage->users.insert( client->sessionId );
				//мьютекс использования клиентов можно не блокировать - сообщения для каждого отдельно взятого клиента обрабатываются по-очереди:
				if ( client->type == Client::IMMORTAL )
					immortalClientMutex.lock();
				client->batchesUnderUsage.batches.insert( std::pair< BatchUsageMap::Key, BatchUsageMap::Holder >( key, BatchUsageMap::Holder( usage ) ) );
				if ( client->type == Client::IMMORTAL )
					immortalClientMutex.unlock();
			}
			
			//уже используются:
			if ( usage->underUsage )
			{
				//добавляем ещё одного пользователя:
				ContextLock* contextLock = new ContextLock;
				usage->requesters.push_back( contextLock );
				//ожижадение получения данных - да ещё и из очереди - может быть очень долгим:
				batchesMutex.unlock();
				contextLock->Stop();
				return BatchRef( usage->batch, OnBatchDestruction, new BatchRefOpaqueData( this, usage ) );
			}
			//ещё не используются:
			else
			{
				//теперь используются:
				usage->underUsage = true;
				
				batchesMutex.unlock();
				
				return BatchRef( usage->batch, OnBatchDestruction, new BatchRefOpaqueData( this, usage ) );
			}
		}
	}
	//запрашиваемых данных ещё не существует - их нужно создать:
	else
	{
		//создаём новые локальные данные:
		BatchUsageMap::Usage* usage = new BatchUsageMap::Usage( NULL, true, batchId );
		
		BatchUsageMap::Holder holder( usage );
		batchUsageMap.batches.insert( std::pair< BatchUsageMap::Key, BatchUsageMap::Holder >( key, holder ) );
		
		//указываем что данные используются текущим клиентом (это нормально что использование добавляется без проверки - в итоге будет лишь один экземпляр):
		{
			usage->users.insert( client->sessionId );
			//мьютекс использования клиентов можно не блокировать - сообщения для каждого отдельно взятого клиента обрабатываются по-очереди:
			if ( client->type == Client::IMMORTAL )
				immortalClientMutex.lock();
			client->batchesUnderUsage.batches.insert( std::pair< BatchUsageMap::Key, BatchUsageMap::Holder >( key, BatchUsageMap::Holder( usage ) ) );
			if ( client->type == Client::IMMORTAL )
				immortalClientMutex.unlock();
		}
		
		batchesCount++;
		
		//процесс создания данных может быть очень долгим - освобождаем возможность использования остальных данных на это время:
		batchesMutex.unlock();
		
		//создаём сами данные:
		usage->batch = batchFactory->CreateCustomBatch( batchId );
		usage->batch->OnCreation( batchId, batchTime_Imp );
		
		return BatchRef( usage->batch, OnBatchDestruction, new BatchRefOpaqueData( this, usage ) );
	}
}

#ifndef FOS_NO_DB

/** DB interface.*/

/** Багажные данные для запроса в базу данных. Например при создании данных или вызове StartingHandler.*/
class DBRequestOpaqueData
{
public:
	DBRequestOpaqueData( ContextLock* contextLock ): contextLock( contextLock )
	{
	}
	
	/** Поток, ожидающий получения данных.*/
	ContextLock* contextLock;
	/** Куда будут помещены полученные данные.*/
	db::ResponseRef responseRef;
};

void DBRequestCallback( const db::ResponseRef& responseRef, void* opaqueData )
{
	DBRequestOpaqueData* dbRequestOpaqueData = ( DBRequestOpaqueData* ) opaqueData;
	dbRequestOpaqueData->responseRef = responseRef;
	dbRequestOpaqueData->contextLock->Resume();
}

db::ResponseRef Dispatcher::DBRequest( char* query, const bool use, uint32_t size )
{
	char* usingQuery = query;
	if ( !use )
	{
		if ( size == 0 )
			size = strlen( query );
		usingQuery = ( char* ) ( char* ) ( char* ) ( char* ) ( char* ) ( char* ) ( char* ) ( char* ) ( char* ) malloc( size + 1 );
		if ( usingQuery == NULL )
		{
			Globals::messenger->write( boost::format( "E: Dispatcher::DBRequest(): memory allocation failed. Request cannot be fulfilled.\n" ) );
			return db::ResponseRef( NULL );
		}
		memcpy( usingQuery, query, size );
		usingQuery[ size ] = '\0';
	}
	
	ContextLock* contextLock = new ContextLock;
	
	DBRequestOpaqueData dbRequestOpaqueData( contextLock );
	
	pqlDbConn->SendQuery( usingQuery, DBRequestCallback, &dbRequestOpaqueData );
	
	//ждём пока придёт ответ из базы данных:
	contextLock->Stop();
	
	return dbRequestOpaqueData.responseRef;
}

db::ResponseRef Dispatcher::DBRequest( const char* query )
{
	return DBRequest( ( char* ) query, false, 0 );
}

db::ResponseRef Dispatcher::DBRequest( const std::string& query )
{
	return DBRequest( ( char* ) query.c_str(), false, ( uint32_t ) query.size() );
}

db::ResponseRef Dispatcher::DBRequest( const boost::format& query )
{
	return DBRequest( str( query ) );
}

void Dispatcher::DBRequestA( char* query, const bool use, uint32_t size )
{
	char* usingQuery = query;
	if ( !use )
	{
		if ( size == 0 )
			size = strlen( query );
		usingQuery = ( char* ) malloc( size + 1 );
		if ( usingQuery == NULL )
		{
			Globals::messenger->write( boost::format( "E: Dispatcher::DBRequestA(): memory allocation failed. Request cannot be fullfilled.\n" ) );
			return;
		}
		memcpy( usingQuery, query, size );
		usingQuery[ size ] = '\0';
	}
	pqlDbConn->SendQuery( usingQuery );
}

void Dispatcher::DBRequestA( const char* query )
{
	DBRequestA( ( char* ) query, false );
}

void Dispatcher::DBRequestA( const std::string& query )
{
	DBRequestA( ( char* ) query.c_str(), false, query.size() );
}

void Dispatcher::DBRequestA( const boost::format& query )
{
	DBRequestA( str( query ) );
}

void Dispatcher::DBAddFilter( const char* msg )
{
	pqlDbConn->AddFilter(msg);
}

#endif//#ifndef FOS_NO_DB


/** Clients events.*/

void Dispatcher::Send(const Outcoming& outcoming)
{
	clientsConnIFace->Send(outcoming);
}

void Dispatcher::Send(const uint64_t& sessionId, const OutStream& outStream)
{
	clientsConnIFace->Send(sessionId, outStream);
}

void Dispatcher::Send(const std::vector<uint64_t>& ids, const OutStream& outStream)
{
	clientsConnIFace->Send(ids, outStream);
}

void Dispatcher::Send(const uint64_t& sessionId, const std::string& data)
{
	clientsConnIFace->Send(sessionId, data);
}

void Dispatcher::Send(const uint64_t& sessionId, const void* data, const size_t length)
{
	clientsConnIFace->Send(sessionId, data, length);
}

const uint64_t& Dispatcher::HandleConnection(const std::string& ipAddress)
{
	lastSessionId++;
	Client::Event* event = new Client::Event(Client::Event::CONNECTION);
	event->ipAddress = new std::string(ipAddress);
	HandleEvent(lastSessionId, event);
	return lastSessionId;
}

const uint64_t& Dispatcher::HandleConnection(const char* ipAddress)
{
	lastSessionId++;
	Client::Event* event = new Client::Event(Client::Event::CONNECTION);
	event->ipAddress = new std::string(ipAddress);
	HandleEvent(lastSessionId, event);
	return lastSessionId;
}

void Dispatcher::HandleMessage(const uint64_t& sessionId, const size_t dataLength, const char* messageData)
{
	Client::Event* event = new Client::Event(Client::Event::MESSAGE);
	event->messageData = (char*)malloc(dataLength);
	event->messageLength = dataLength;
	memcpy(event->messageData, messageData, dataLength);
	HandleEvent(sessionId, event);
}

void Dispatcher::HandleDisconnection(const uint64_t& sessionId)
{
	HandleEvent(sessionId, new Client::Event(Client::Event::DISCONNECTION));
}

void Dispatcher::HandleEvent( const uint64_t& sessionId, Client::Event* event )
{
	clientsMutex.lock();
	
	std::map< uint64_t, Client* >::iterator it = clients.find( sessionId );
	if ( it != clients.end() )
	{
		if ( event->type == Client::Event::CONNECTION )
			Globals::messenger->write(boost::format("E: Dispatcher::HandleEvent(): CONNECTION: client %lld already exists.\n") % sessionId);
		
		Client* client = it->second;
		
		//для "встроенных" клиентов сообщения обрабатываться не должны (хотя могут поступать по инициативе клиентского кода):
		if ( client->type == Client::IMMORTAL && event->type != Client::Event::DROP_BATCH )
		{
			delete event;
			clientsMutex.unlock();
			return;
		}
		
		//если какое-либо событие, связанное с этим клиентом, уже обрабатывается в отдельном потоке, то текущее событие обработается из очереди там же:
		if ( client->isOnHandle )
		{
			client->events.push_back(event);
		}
		else
		{
			client->isOnHandle = true;
			
			handlersPoolMutex.lock();
			handlersPool.schedule( boost::bind( &Dispatcher::HandlerRunner, this, client, event ) );
			handlersPoolMutex.unlock();
		}
	}
	//первое событие от клиента:
	else
	{
		//события могут быть получены и после отключения клиента (или даже без его предыдущего подключения), поэтому для несуществующего клиента обрабатываем только событие подключения:
		if ( event->type == Client::Event::CONNECTION )
		{
			Client* client = new Client( Client::ORDINARY, sessionId, true, NULL, *( event->ipAddress ) );
			
			clients.insert( std::pair< uint64_t, Client* >( client->sessionId, client ) );
			
			handlersPoolMutex.lock();
			handlersPool.schedule( boost::bind( &Dispatcher::HandlerRunner, this, client, event ) );
			handlersPoolMutex.unlock();
		}
		else
		{
			//событие удаляния данных может для несуществующих клиентов можент существляться как по неосторожности клиентского кода, так и по race-condition'у (клиент может успеть уже удалиться):
			if ( event->type != Client::Event::DROP_BATCH )
				Globals::messenger->write( boost::format( "W: Dispatcher::HandleEvent(): message of type %d for inexistent client under session id = %llu.\n" ) % ( int ) event->type % sessionId );
			delete event;
		}
	}
	
	clientsMutex.unlock();
}

void Dispatcher::HandlerRunner(Client* client, Client::Event* event)
{
	//если это первое событие от клиента:
	if ( client->luggage == NULL )
	{
		client->luggage = batchFactory->CreateLuggageBatch( client->sessionId );
		IntegralBatchId integralBatchId( client->sessionId );
		client->luggage->OnCreation( &integralBatchId, batchTime_Imp );
	}
	
	HandleEventSync( client, event );
	
	//если было сообщение DISCONNECT:
	if ( client == NULL )
		return;
	
	//выполнение ожидающих событий:
	for( ;; )
	{
		clientsMutex.lock();
		
		if( client->events.empty() )
		{
			client->isOnHandle = false;
			clientsMutex.unlock();
			break;
		}
		else
		{
			Client::Event* event = client->events.front();
			client->events.pop_front();
			
			//обработка события может быть очень долгой (синхронные запросы в базу данных, получение данных...), поэтому освобождаем возможность добавления новых событий:
			clientsMutex.unlock();
			
			HandleEventSync( client, event );
			//после сообщения DISCONNECT не должно быть никаких сообщений, но, на-всякий случай, проверяем:
			if ( client == NULL )
				break;
		}
	}
}

#ifndef FOS_NO_DB
std::string Dispatcher::GetDBStats()
{
	boost::interprocess::scoped_lock<boost::mutex> lock(pqlDbConn->statsMutex);
	
	return str(boost::format("gross requests: %llu;\ngross seconds: %.6f;\nseconds/requests: %10.6f;\nlongest request: %10.6f seconds, text: \"%s\";\nfastest request: %10.6f seconds, text: \"%s\".\n") % pqlDbConn->requestsCount % pqlDbConn->grossSeconds % (pqlDbConn->grossSeconds / (double)pqlDbConn->requestsCount) % pqlDbConn->longestRequestSeconds % pqlDbConn->longestRequest % pqlDbConn->fastestRequestSeconds % pqlDbConn->fastestRequest);
}

#endif//#ifndef FOS_NO_DB

size_t Dispatcher::GetCurrentConnectionsCount()
{
	boost::interprocess::scoped_lock<boost::mutex> lock(clientsMutex);
	
	return clients.size();
}

}//namespace fos

#include "memdbg_end.h"



