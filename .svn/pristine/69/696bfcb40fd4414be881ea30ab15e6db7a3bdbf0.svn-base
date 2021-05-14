

/** \manpage Многопоточный сервер, используемый по методу Голливуда: "Don't call us, we'll call you!".
TODO - описание.
*/


#ifndef FOS_DISPATCHER_H
#define FOS_DISPATCHER_H

#include "batch.h"

#include <boost/cstdint.hpp>
#include <boost/format.hpp>

#ifndef FOS_NO_DB
#include "db/pql.h"
#endif

#include <list>
#include <set>

#include "boost/threadpool.hpp"

#include "outcoming.h"

#include "batch_usage_map.h"

#include "handlers.h"

#include "times/batch_time_imp.h"
#include "times/client_time_imp.h"
#include "times/common_time_imp.h"

#include "client.h"

#include "externs.h"

#include "meters_factory.h"


namespace fos
{


class ClientsConnIFace;
class BatchFactory;
class BatchRef;

/** Ожидание подключений от клиентов. Обработка сообщений от клиентов. База данных.
Поток создания НЕ блокирует - вся работа происходит в отдельном потоке (может быть в нескольких - сотнях).*/
class Dispatcher
{
public:
	
#ifdef FOS_NO_DB
	Dispatcher(MetersFactory* metersFactory, 
			BatchFactory* batchFactory, 
			StartingHandler startingHandler, 
			TickHandler tickHandler, 
			const double tickPeriod, 
			ConnectionHandler connectionHandler, 
			MessageHandler messageHandler, 
			DisconnectionHandler disconnectionHandler, 
			ExitingHandler exitingHandler, 
			const int clientsPort, 
			void* clientData);
#else
	Dispatcher(const char* dbName, 
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
			const int clientsPort, 
			void* clientData);
#endif//#ifdef FOS_NO_DB
	
	~Dispatcher();
	
public:
	
	void Construct(MetersFactory* metersFactory, StartingHandler startingHandler, TickHandler tickHandler, const double tickPeriod, const int clientsPort, void* clientData);
	
	
	/** Clients interface.*/
	
	/** Получить слудующий идентификатор сессии для очередного входящего подключения. Обработка события.
	\param sessionId Идентификатор сессии, по которому можно будет отправлять клиенту сообщения.
	\param ipAddress IP-адрес подключившегося клиента.*/
	const uint64_t& HandleConnection(const std::string& ipAddress);
	const uint64_t& HandleConnection(const char* ipAddress);
	
	/** Получение сообщения от клиента. Обработка события.*/
	void HandleMessage(const uint64_t& sessionId, const size_t dataLength, const char* messageData);
	
	/** Отключение клиента с указанным идентификатором сессии. Обработка события.*/
	void HandleDisconnection(const uint64_t& sessionId);
	
	
	/** User interface.*/
	
	/** Отправка сообщения.*/
	void Send(const Outcoming& outcoming);
	void Send(const uint64_t& sessionId, const OutStream& outStream);
	void Send(const std::vector<uint64_t>& ids, const OutStream& outStream);
	void Send(const uint64_t& sessionId, const std::string& data);
	void Send(const uint64_t& sessionId, const void* data, const size_t length);
	
#ifndef FOS_NO_DB
	/** Синхронный запрос в базу данных.*/
	/** Если use = true, то используется переданная область памяти при отправке запроса и освобождается при получении результата запроса.
	\param size Указать если известен размер строки БЕЗ символа конца строки. Если размер неизвестен, то 0.*/
	db::ResponseRef DBRequest( char* query, const bool use, uint32_t size = 0 );
	/** Данные для запроса копируются.*/
	db::ResponseRef DBRequest( const char* query );
	db::ResponseRef DBRequest( const std::string& query );
	db::ResponseRef DBRequest( const boost::format& query );
	/** Отправка запроса в базу данных без ожидания ответа от неё.*/
	/** Если use = true, то используется переданная область памяти при отправке запроса и освобождается при получении результата запроса.
        \param size Указать если известен размер строки БЕЗ символа конца строки. Если размер неизвестен, то 0.*/
	void DBRequestA( char* query, const bool use, uint32_t size = 0 );
	/** Данные для запроса копируются.*/
	void DBRequestA( const char* query );
	void DBRequestA( const std::string& query );
	void DBRequestA( const boost::format& query );
	/** Добавить фильтр вывода сообщений об ошибках из базы данных. Если сообщение об ошибке содержит указанную подстроку, то сообщение выводиться не будет.*/
	void DBAddFilter(const char* msg);
#endif//#ifndef FOS_NO_DB
	
	/** Синхронное получение данных.*/
	BatchRef GetBatch(Client* client, const BatchId* batchId);

	/** Освобождение данных, используемое по различной инициативе.*/
	void ReleaseBatch_ThreadUnsafe(BatchUsageMap::Usage* usage);
	
	/** Освобождение данных от внутреннего использования.*/
	void ReleaseBatch(BatchUsageMap::Usage* usage);
	
	/** Освободить данные от использования.*/
	void DropBatch(Client* client, const BatchId* batchId);
	/** Освободить данные от использвания клиента, указанного посредством sessionId.*/
	void DropBatch(const uint64_t sessionId, const BatchId* batchId);
	
	/** Удалить (отключить) клиента(-ов).*/
	void Disconnect(const uint64_t& id);
	void Disconnect(const std::vector<uint64_t>& ids);
	
	/** Возвращает список все подключённых в данный момент клиентов.*/
	boost::shared_ptr<std::vector<uint64_t> > GetIds();
	
	/** Выделяет интерфейс с сервером, который можно использовать где угодно и когда угодно. Должен быть удалён самостоятельно.*/
	CommonTime* AllocateCommonTime();
	
	
	
	/** Обработать входящее от клиента событие.*/
	void HandleEvent(const uint64_t& sessionId, Client::Event* event);
	

	/** Потоковая функция вызова обработчика и проверки входящих событий, ожидающих обработки.*/
	void HandleClientConnection(Client* client, Client::Event* event);
	void HandlerRunner(Client* client, Client::Event* event);
	inline void HandleEventSync(Client*& client, Client::Event* event);
	/** Освобождение данных от использования указанным клиентом.*/
	inline void ReleaseBatches(Client* client);
	/** Освободить данные от использования их указанным клиентом.*/
	inline void ReleaseClientsBatch_TU(Client* client, const BatchId* batchId);

	/** Периодический вызов TickHandler'а.*/
	void TickRunner(TickHandler tickHandler, const double period);
	
#ifndef FOS_NO_DB
	std::string GetDBStats();
#endif
	
	size_t GetCurrentConnectionsCount();
	
	/** Пул потоков обработки сообщений (а также событий с данными).*/
	boost::threadpool::pool handlersPool;
	
	
	/** Все имеющиеся на этом сервере клиенты.*/
	std::map<uint64_t, Client*> clients;
	/** При использовании клиентов.*/
	boost::mutex clientsMutex;
	
	
	
	/** Переданная извне фабрика создания данных.*/
	BatchFactory* batchFactory;
	
	ClientsConnIFace* clientsConnIFace;
	
#ifndef FOS_NO_DB
	pq_db::PqlDbConn* pqlDbConn;
#endif
	
	
	/** Интерфейс для создания данных.*/
	BatchTime_Imp* batchTime_Imp;
	
	
	/** Все имеющиеся пользовательские данные (за исключением luggages).*/
	BatchUsageMap batchUsageMap;
	/** При использовании данных.*/
	boost::mutex batchesMutex;
	
	/** Переданный извне обработчик пользовательских сообщений.*/
	ConnectionHandler connectionHandler;
	MessageHandler messageHandler;
	DisconnectionHandler disconnectionHandler;
	ExitingHandler exitingHandler;

	/** Эмуляция пользователя данных для получения оных из самого диспетчера.*/
	Client immortalClient;
	boost::mutex immortalClientMutex;
	
	/** Последний идентификатор сессии, присвоенные какому-либо клиенту.*/
	uint64_t lastSessionId;
	
	/** Хранится здесь только для удаления в деструкторе.*/
	ClientsHandler* clientsHandler;
	
	/** Попробуем избавиться от предупреждения DRD.*/
	boost::mutex handlersPoolMutex;
	
	/** true - сервер в данный момент находится в процессе завершения своей работы.*/
	bool stoppingServer;
	bool tickRunnerExited;
	
	/** Снова некое недоразумение нужно только для остановки сервера - необходимо ждать завершения удаления всех batch'ей перед остановкой сервера, ведь их удаления совершаются также в параллельных потоках.*/
	int batchesCount;
};

void Dispatcher::ReleaseBatches(Client* client)
{
 	batchesMutex.lock();
	
	for ( BatchUsageMap::Iterator it = client->batchesUnderUsage.batches.begin(); it != client->batchesUnderUsage.batches.end(); ++it )
	{
		BatchUsageMap::Usage* usage = it->second.usage;
		
		if ( usage == NULL )
		{
			Globals::messenger->write("E: Dispatcher::ReleaseBatches(): NULL usage.\n");
			continue;
		}
		
		const int erased = usage->users.erase(client->sessionId);
		if ( erased <= 0 )
			Globals::messenger->write("E: Dispatcher::ReleaseBatches(): usage|under cross failed.\n");

		//если больше не осталось пользователей, то данные нужно удалить:
		if ( usage->users.empty() )
		{
			if ( !usage->underUsage )
			{
				if ( usage->requesters.empty() )
				{
					BatchUsageMap::Key key(usage->batchId, BatchUsageMap::Key::SHARED);
					if ( batchUsageMap.batches.erase(key) != 1 )
						Globals::messenger->write("E: Dispatcher::ReleaseBatches(): usage was NOT erased.\n");
					batchesMutex.unlock();
					usage->batch->OnDestruction(batchTime_Imp);
					delete usage;
					batchesMutex.lock();
					batchesCount--;
				}
				else
					Globals::messenger->write("E: Dispatcher::ReleaseBatches(): queued unused Usage.\n");
			}
		}
	}
	
	batchesMutex.unlock();
}

void Dispatcher::HandleEventSync(Client*& client, Client::Event* event)
{
	switch ( event->type )
	{
		case Client::Event::CONNECTION:
		{
			ClientTime_Imp clientTime_Imp(this, client);
			connectionHandler(client->sessionId, client->luggage, *(event->ipAddress), &clientTime_Imp);
		}
		break;
		
		case Client::Event::MESSAGE:
		{
			ClientTime_Imp clientTime_Imp(this, client);
			messageHandler(client->sessionId, event->messageData, event->messageLength, client->luggage, &clientTime_Imp);
		}
		break;
		
		case Client::Event::DISCONNECTION:
		{
			//удаление сессии клиента, багажа и всех данных, связанных с ним:
			
			ClientTime_Imp clientTime_Imp(this, client);
			disconnectionHandler(client->sessionId, client->luggage, &clientTime_Imp);
			
			client->luggage->OnDestruction(batchTime_Imp);
			delete client->luggage;
			
			//мьютекс лочить только здесь (не выше строки с вызовом disconnectionHandler'а), иначе может возникнуть deadlock внутри disconnectionHandler'е при использовании методов предоставленного ему интерфейса:
			clientsMutex.lock();
			
			clients.erase(client->sessionId);
			//удаляем все события клиента, которые могли накопиться после сообщения отключения:
			for ( size_t i = 0; i < client->events.size(); i++ )
			{
				delete client->events[i];
			}
			//вектор events можно не очищать...
			
			clientsMutex.unlock();
			
			ReleaseBatches(client);
			
			delete client;
			client = NULL;
		}
		break;
		
		case Client::Event::DROP_BATCH:
		{
			ReleaseClientsBatch_TU(client, event->batchId);
			
			delete event;
			event = NULL;
		}
		break;
	}
	
	delete event;
}

void Dispatcher::ReleaseClientsBatch_TU( Client* client, const BatchId* batchId )
{
	//ищем освобождаемые данные:
	BatchUsageMap::Key key( batchId, BatchUsageMap::Key::SHARED );
	batchesMutex.lock();
	BatchUsageMap::Iterator it = client->batchesUnderUsage.batches.find( key );
	if ( it != client->batchesUnderUsage.batches.end() )
	{
		BatchUsageMap::Usage* usage = it->second.usage;
		if ( usage != NULL )
		{
			if ( usage->users.erase(client->sessionId) <= 0 )
				Globals::messenger->write( "E: Dispatcher::ReleaseClientsBatch_TU(): cross absence.\n" );
			
			ReleaseBatch_ThreadUnsafe( usage );
		}
		else
		{
			Globals::messenger->write( "E: Dispatcher::ReleaseClientsBatch_TU(): usage is NULL.\n" );
		}
		
		//удаляем данные из использования только уже после использования итератора (accordingly debug assertion):
		 client->batchesUnderUsage.batches.erase( it );
	}
	else
	{
		Globals::messenger->write( "W: Dispatcher::ReleaseClientsBatch_TU(): NOT found.\n" );
	}
	batchesMutex.unlock();
}

}//namespace fos


#endif//#ifndef FOS_DISPATCHER_H

