

#include "clients_handler_aio.h"

#include "boost/thread.hpp"

#include "../collector/leb128_collector.h"

#include "../../clients_conn_iface.h"


#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>


namespace fos
{

void handle_read(union sigval);

void handle_write(union sigval);

class ClientConn;

class AioIncomingDataHandler: public IncomingDataHandler
{
public:
	AioIncomingDataHandler(const uint64_t& sessionId, ClientsConnIFace* clientsConnIFace): sessionId(sessionId), clientsConnIFace(clientsConnIFace)
	{
	}
	
	void HandleMessage(const size_t length, const void* buffer)
	{
		clientsConnIFace->HandleMessage(sessionId, length, (const char*)buffer);
	}
	
private:
	uint64_t sessionId;
	ClientsConnIFace* clientsConnIFace;
};

/** Для доступа к списку подключённых пользователей из handle_read().*/
class ReadingSival
{
public:
	ReadingSival(ClientsHandler_Aio* clientsHandler_Aio, ClientConn* clientConn): clientsHandler_Aio(clientsHandler_Aio), clientConn(clientConn)
	{
	}
	
	/** Для обрабтки отключений и передачи входящих сообщений.*/
	ClientsHandler_Aio* clientsHandler_Aio;
	ClientConn* clientConn;
};

/** Буфер данных для отправки. Может использоваться сразу несколькими отправками (разным пользователям).*/
class WritingBufferRef
{
public:
	WritingBufferRef(void* buffer, const size_t usageCount, const size_t bufferSize): buffer(buffer), usageCount(usageCount), bufferSize(bufferSize)
	{
	}
	
	/** Освободить очередную ссылку.*/
	void Decrement()
	{
		usageCount--;
		if(usageCount <= 0)
		{
			free(buffer);
			delete this;
		}
	}
	
	void* buffer;
	size_t usageCount;
	size_t bufferSize;
};

class WritingSival
{
public:
	WritingSival(ClientsHandler_Aio* clientsHandler_Aio, ClientConn* clientConn, WritingBufferRef* writingBufferRef): clientsHandler_Aio(clientsHandler_Aio), clientConn(clientConn), writingBufferRef(writingBufferRef)
	{
	}
	
	/** Для обработки отключений и использования мьютекса использования writingBuffer.*/
	ClientsHandler_Aio* clientsHandler_Aio;
	/** Для проверки удаления или продолжения записи.*/
	ClientConn* clientConn;
	
	/** Буфер записи, который используется для этого соединения.*/
	WritingBufferRef* writingBufferRef;
};

/** Data associated with single client (socket) connection.*/
class ClientConn
{
public:
	
	ClientConn(const int readingSocketDesc, const int writingSocketDesc, const uint64_t& sessionId, ClientsHandler_Aio* clientsHandler_Aio): readingSocketDesc(readingSocketDesc), writingSocketDesc(writingSocketDesc), sessionId(sessionId), isReading(true), isWriting(false), isRemoving(false), issueDisconnection(true), readingSival(clientsHandler_Aio, this)
	{
		memset(&readingAiocb, 0, sizeof(struct aiocb));
		readingAiocb.aio_nbytes = BUFFER_SIZE;
		readingAiocb.aio_fildes = readingSocketDesc;
		readingAiocb.aio_buf = readBuffer;
		readingAiocb.aio_sigevent.sigev_notify = SIGEV_THREAD;
		readingAiocb.aio_sigevent.sigev_value.sival_ptr = (void*)&readingSival;
		readingAiocb.aio_sigevent.sigev_notify_function = handle_read;
		
		memset(&writingAiocb, 0, sizeof(struct aiocb));
		writingAiocb.aio_fildes = writingSocketDesc;
		writingAiocb.aio_sigevent.sigev_notify = SIGEV_THREAD;
		writingAiocb.aio_sigevent.sigev_notify_function = handle_write;
	}
	
	void RemoveWrites()
	{
		for(std::deque<WritingSival*>::iterator it = writingSivals.begin(); it != writingSivals.end(); it++)
		{
			(*it)->writingBufferRef->Decrement();
			delete *it;
		}
	}
	
	const int readingSocketDesc;
	const int writingSocketDesc;
	
	static const int BUFFER_SIZE = 8192;
	/** Куда считываются входящие сообщения.*/
	char readBuffer[BUFFER_SIZE];
	
	struct aiocb readingAiocb;
	
	/** Идентификатор клиента внутри сервера для связки получаемых и отправляемых сообщений.*/
	uint64_t sessionId;
	
	/** Текуще выполняемая запись в сокет, если isWriting == true.*/
	struct aiocb writingAiocb;
	/** Данные намеченных записей в сокет. FIFO. Данные записи, асинхронный результат которой ожидается в данный момент, НЕ находится в очередь - они будут переданы в handle_read() в качестве параметра.*/
	std::deque<WritingSival*> writingSivals;
	
	/** true - осуществляется чтение и вот-вот будет вызван обработчик - объект удалять нельзя - произойдёт segmentation fault внутри обработчика чтения.*/
	bool isReading;
	/** true - объект поставлен на удаление - в следующем обработчике чтения он будет удалён.*/
	bool isRemoving;
	/** true - вот-вот будет вызвана функция handle_write().*/
	bool isWriting;
	/** true - нужно сообщить об отключении клиента.*/
	bool issueDisconnection;
	
	/** Используется вообще при любом использовании этого соединения.*/
	boost::mutex usageMutex;
	
	/** Передаётся в handle_read().*/
	ReadingSival readingSival;
	
	LEB128Collector leb128Collector;
};

/** Handles asynchronous read operations.*/
void handle_read(union sigval sigev_value)
{
	//Globals::messenger->write(boost::format("I: handle_read(): start.\n"));
	
	ReadingSival* readingSival = (ReadingSival*)sigev_value.sival_ptr;
	ClientConn* clientConn = readingSival->clientConn;
	
	clientConn->usageMutex.lock();
	
	bool keepReading = false;
	
	//если клиента "попросили" удалить:
	if(clientConn->isRemoving)
	{
		//Globals::messenger->write(boost::format("I: handle_read(): removing client %llu.\n") % clientConn->sessionId);
		
		//в этом случае (isRemoving = true) соединение уже удалено из списка, поэтому этого делать больше не нужно...
		
		//если клиента можно удалять прямо сейчас:
		if(!clientConn->isWriting)
		{
			//Globals::messenger->write(boost::format("I: handle_read(): removing client right now.\n"));
			
			//удаляем все назначенные записи:
			clientConn->RemoveWrites();
		
			//мьютекс нужно освобождать перед удалением:
			clientConn->usageMutex.unlock();
			
			if(clientConn->issueDisconnection)
			{
				readingSival->clientsHandler_Aio->clientsConnIFace->HandleDisconnection(clientConn->sessionId);
			}
			
			delete clientConn;
			
			readingSival->clientsHandler_Aio->DecrementClientsCount();
		}
	}
	//нормальное функционирование:
	else if(clientConn->isReading)
	{
		bool remove = false;
		
		const int readingErrno = aio_error(&(clientConn->readingAiocb));
		//"Connection reset by peer" случается при отключении со стороны клиента (вполне приемлемая ситуация):
		if(readingErrno != 0)
		{
			//Globals::messenger->write(boost::format("E: handle_read(): reading failed with error \"%s\". Removing connection.\n") % strerror(readingErrno));
			remove = true;
		}
		else
		{
			const int readBytes = aio_return(&(clientConn->readingAiocb));
			//some error:
			if(readBytes < 0)
			{
				Globals::messenger->write(boost::format("E: handle_read(): read %d bytes. Keep reading...\n") % readBytes);
				keepReading = true;
			}
			else if(readBytes == 0)
			{
				//мьютекс можно освобождать уже здесь - соединение не будет удалено пока isReading ещё true (а записи могут безопасно добавляться в очередь):
				//Globals::messenger->write(boost::format("I: handle_read(): got zero bytes. Removing connection...\n"));
				
				remove = true;
			}
			//some incoming data:
			else
			{
				//мьютекс можно освобождать уже здесь - соединение не будет удалено пока isReading ещё true (а записи могут безопасно добавляться в очередь):
				clientConn->usageMutex.unlock();
				
				//Globals::messenger->write(boost::format("I: handle_read(): got %d bytes.\n") % readBytes);
				
				AioIncomingDataHandler aioIncomingDataHandler(clientConn->sessionId, readingSival->clientsHandler_Aio->clientsConnIFace);
				clientConn->leb128Collector.CollectData(readBytes, clientConn->readBuffer, &aioIncomingDataHandler);
				
				keepReading = true;
			}
		}
		
		if(remove)
		{
			clientConn->isRemoving = true;
			
			//удаляем соединение из списка:
			//для этого приосвобождаем мьютекс соединения чтобы в сторонних потоках могли быть добавлены записи в ещё имеющееся в списке соединение:
			clientConn->usageMutex.unlock();
			
			readingSival->clientsHandler_Aio->clientsMutex.lock();
			if(readingSival->clientsHandler_Aio->clients.erase(clientConn->sessionId) != 1)
			{
				Globals::messenger->write(boost::format("E: handle_read(): client %llu was NOT erased.\n") % clientConn->sessionId);
			}
			readingSival->clientsHandler_Aio->clientsMutex.unlock();
			
			clientConn->usageMutex.lock();
			//если нужно ждать пока соединение будет завершится запись:
			if(clientConn->isWriting)
			{
				clientConn->usageMutex.unlock();
			}
			//в противном случае соединение можно удалить прямо сейчас:
			else
			{
				//теперь соединения нет в списке и никаких записей в него добавить уже нельзя - его можно удалять:
				clientConn->RemoveWrites();
				
				//мьютекс нужно освобождать перед удалением:
				clientConn->usageMutex.unlock();
				
				if(clientConn->issueDisconnection)
				{
					readingSival->clientsHandler_Aio->clientsConnIFace->HandleDisconnection(clientConn->sessionId);
				}
				
				delete clientConn;
				
				readingSival->clientsHandler_Aio->DecrementClientsCount();
			}
		}
		
		//здесь какие-либо данные (в том числе и мьютекс) пользователя использовать уже нельзя - он может быть удалён...
	}
	else
	{
		Globals::messenger->write(boost::format("I: handle_read(): no reading and no removing - ??.\n"));
	}
	
	if(keepReading)
	{
		if(aio_read(&(clientConn->readingAiocb)) != 0)
		{
			Globals::messenger->write(boost::format("E: handle_read(): aio_read() failed with error \"%s\".\n") % strerror(errno));
		}
	}
	
	//readingSival является полем объекта класса ClientConn, для которого сейчас осуществилось чтение, поэтому его удалять не нужно...
	
	//Globals::messenger->write(boost::format("I: handle_read(): end.\n"));
}

void handle_write(union sigval sigev_value)
{
	//Globals::messenger->write(boost::format("I: handle_write(): start.\n"));
	
	WritingSival* writingSival = (WritingSival*)sigev_value.sival_ptr;
	ClientConn* clientConn = writingSival->clientConn;
	ClientsHandler_Aio* clientsHandler_Aio = writingSival->clientsHandler_Aio;
	
	clientsHandler_Aio->writingBufferRefMutex.lock();
	writingSival->writingBufferRef->Decrement();
	clientsHandler_Aio->writingBufferRefMutex.unlock();
	
	clientConn->usageMutex.lock();
	
	if(clientConn->isRemoving)
	{
		//если соединение можно удалить прямо сейчас:
		if(!clientConn->isWriting)
		{
			clientConn->RemoveWrites();
			
			//мьютекс нужно освобождать перед удалением:
			clientConn->usageMutex.unlock();
			
			if(clientConn->issueDisconnection)
			{
				clientsHandler_Aio->clientsConnIFace->HandleDisconnection(clientConn->sessionId);
			}
			
			delete clientConn;
			
			clientsHandler_Aio->DecrementClientsCount();
		}
	}
	else
	{
		bool remove = false;
		const int writingErrno = aio_error(&(clientConn->writingAiocb));
		if(writingErrno != 0)
		{
			Globals::messenger->write(boost::format("E: handle_write(): writing failed with error \"%s\". Removing connection...\n") % strerror(writingErrno));
			remove = true;
		}
		else
		{
			const int wroteBytes = aio_return(&(clientConn->writingAiocb));
			if(wroteBytes < 0)
			{
				Globals::messenger->write(boost::format("E: handle_write(): wroteBytes = %d is less than zero. Removing connection...\n") % wroteBytes);
				remove = true;
			}
			else if(wroteBytes == 0)
			{
				Globals::messenger->write(boost::format("E: handle_write(): wrote 0 bytes. Connection closed by peer?\n"));
				remove = true;
			}
			else if(wroteBytes < clientConn->writingAiocb.aio_nbytes)
			{
				Globals::messenger->write(boost::format("E: handle_write(): wroteBytes = %d is LESS than intended %d bytes. Removing connection...\n") % wroteBytes % (clientConn->writingAiocb.aio_nbytes));
				remove = true;
			}
			//нормальная успешная запись:
			else if(wroteBytes == clientConn->writingAiocb.aio_nbytes)
			{
				//Globals::messenger->write(boost::format("I: handle_write(): %d bytes successfully written.\n") % wroteBytes);
				
				//если всё что нужно уже отправлено:
				if(clientConn->writingSivals.empty())
				{
					clientConn->isWriting = false;
				}
				//если есть что-либо для отправки:
				else
				{
					WritingSival* writingSival = clientConn->writingSivals.front();
					clientConn->writingSivals.pop_front();
					
					clientConn->writingAiocb.aio_nbytes = writingSival->writingBufferRef->bufferSize;
					clientConn->writingAiocb.aio_buf = writingSival->writingBufferRef->buffer;
					clientConn->writingAiocb.aio_sigevent.sigev_value.sival_ptr = (void*)writingSival;
					
					if(aio_write(&(clientConn->writingAiocb)) != 0)
					{
						Globals::messenger->write(boost::format("E: handle_write(): aio_write() failed with error \"%s\".\n") % strerror(errno));
					}
				}
			
				clientConn->usageMutex.unlock();
			}
			else
			{
				Globals::messenger->write(boost::format("E: handle_write(): wroteBytes = %d is MORE than intended %d bytes. Removing connection...\n") % wroteBytes % (clientConn->writingAiocb.aio_nbytes));
				remove = true;
			}
		}
	
		if(remove)
		{
			//если нужно ждать удаления клиента в handle_read():
			if(clientConn->isReading)
			{
				clientConn->isWriting = false;
				clientConn->isRemoving = true;
				//отменяем чтение - функция handle_read() будет вызвана с ошибкой (что и нужно):
				shutdown(clientConn->readingSocketDesc, SHUT_RD);
				clientConn->usageMutex.unlock();
			}
			//в противном случае соединение можно удалить прямо сейчас:
			else
			{
				//приосвобождаем использование соединения чтобы в других потоках могли отправить (добавить в очередь на отправку) сообщения этому клиенту (чтобы не возникало deadlock'а) и после чего освободить мьютекс использования всех клиентов:
				clientConn->usageMutex.unlock();
				
				clientsHandler_Aio->clientsMutex.lock();
				if(clientsHandler_Aio->clients.erase(clientConn->sessionId) != 1)
				{
					Globals::messenger->write(boost::format("E: handle_write(): client was NOT removed under session id = %llu.\n") % clientConn->sessionId);
				}
				clientsHandler_Aio->clientsMutex.unlock();
		
				//clientConn->usageMutex можно и не блокировать больше - доступ к соединению уже не может быть получен, так как оно удалено из списка...
				
				clientConn->RemoveWrites();
				
				if(clientConn->issueDisconnection)
				{
					clientsHandler_Aio->clientsConnIFace->HandleDisconnection(clientConn->sessionId);
				}
		
				delete clientConn;
				
				clientsHandler_Aio->DecrementClientsCount();
			}
		}
	}
	
	//здесь clientConn использовать уже нельзя - он может быть уже удалён...
	
	delete writingSival;
	
	//Globals::messenger->write(boost::format("I: handle_write(): end.\n"));
}


ClientsHandler_Aio::ClientsHandler_Aio(const int port, ClientsConnIFace* clientsConnIFace): clientsConnIFace(clientsConnIFace), isStopping(false), isRunnerExited(false), isRunning(false), clientsCount(0)
{
	clientsConnIFace->SetClientsHandler(this);
	
	boost::thread runningThread(boost::bind(&ClientsHandler_Aio::Run, this, port));
}

ClientsHandler_Aio::~ClientsHandler_Aio()
{
}

void ClientsHandler_Aio::CancelAcceptingConnections()
{
	isStopping = true;
	
	//остановка обработки новых подключений:
	Globals::messenger->write(boost::format("I: ClientsHandler_Aio::CancelAcceptingConnections(): shutting down accepting socket...\n"));
	runnerMutex.lock();
	if(isRunning)
	{
		//прерываем функцию accept():
		shutdown(acceptingSocket, SHUT_RDWR);
		close(acceptingSocket);
		//ждём когда завершится поток функции Run():
		while(!isRunnerExited)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}
	runnerMutex.unlock();
	Globals::messenger->write(boost::format("I: ClientsHandler_Aio::CancelAcceptingConnections(): accepting socket was successfully shut down.\n"));
}

void ClientsHandler_Aio::DisconnectClients()
{
	//отключаем всех уже существующих пользователей:
	Globals::messenger->write(boost::format("I: ClientsHandler_Aio::DisconnectClients(): removing all current connections.\n"));
	for(;;)
	{
		uint64_t sessionId;
		bool found = false;
		clientsMutex.lock();
		if(!clients.empty())
		{
			sessionId = clients.begin()->first;
			found = true;
		}
		clientsMutex.unlock();
		
		if(found)
		{
			Disconnect_Inner(sessionId, true);
		}
		else
		{
			break;
		}
	}
	Globals::messenger->write(boost::format("I: ClientsHandler_Aio::DisconnectClients(): all current connections successfully removed.\n"));
	
	Globals::messenger->write(boost::format("I: ClientsHandler_Aio::DisconnectClients(): waiting for physical connections removing...\n"));
	//ждём когда все соединения будут физически удалены:
	int previous = -1;
	int times = 0;
	for(;;)
	{
		clientsCountUsageMutex.lock();
		const int clientsCountCopy = clientsCount;
		clientsCountUsageMutex.unlock();
		
		if(clientsCountCopy != previous)
		{
			Globals::messenger->write(boost::format("I: ClientsHandler_Aio::DisconnectClients(): left %d clients...\n") % clientsCountCopy);
			previous = clientsCountCopy;
		}
		
		if(times >= 100)
		{
			Globals::messenger->write(boost::format("E: ClientsHandler_Aio::DisconnectClients(): waiting for physical connections removing for too long - breaking.\n"));
			break;
		}
		
		if(clientsCountCopy > 0)
		{
			times++;
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
		else
		{
			break;
		}
	}
	Globals::messenger->write(boost::format("I: ClientsHandler_Aio::DisconnectClients(): all connections successfully removed.\n"));
}

void ClientsHandler_Aio::Run(const int port)
{
	acceptingSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(acceptingSocket == -1)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Run(): socket() failed with error \"%s\".\n") % strerror(errno));
		return;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if(bind(acceptingSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Run(): bind() failed with error \"%s\".\n") % strerror(errno));
	}
	
	listen(acceptingSocket, SOMAXCONN);
	
	
	struct sockaddr_in address;
	socklen_t addressLen = sizeof(struct sockaddr_in);
	
	runnerMutex.lock();
	//если сервер (так скоро) уже останавливает свою работу, то ничего делать не нужно:
	if(isStopping)
	{
		runnerMutex.unlock();
		return;
	}
	isRunning = true;
	runnerMutex.unlock();
	
	for(;;)
	{
		const int incomingSocket = accept(acceptingSocket, (struct sockaddr*)&address, &addressLen);
		
		//если сервер в данный момент завершает свою работу, то нужно прекратить выполнение:
		if(isStopping)
		{
			break;
		}
		
		if(incomingSocket == -1)
		{
			Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Run(): accept() failed with error \"%s\".\n") % strerror(errno));
		}
		else
		{
			//say socket to append outcoming messages at writing:
			const int currentFlags = fcntl(incomingSocket, F_GETFL, 0);
			if(currentFlags < 0)
			{
				Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Run(): fcntl(F_GETFL) failed with \"%s\".\n") % strerror(errno));
			}
			if(fcntl(incomingSocket, F_SETFL, currentFlags | O_APPEND) == -1)
			{
				Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Run(): fcntl(F_SETFL) failed with \"%s\".\n") % strerror(errno));
			}
			


			const char* ipAddress = inet_ntoa(address.sin_addr);
			
			const int writingSocketDesc = dup(incomingSocket);
			
			clientsCountUsageMutex.lock();
			clientsCount++;
			clientsCountUsageMutex.unlock();
			
			ClientConn* clientConn = new ClientConn(incomingSocket, writingSocketDesc, clientsConnIFace->HandleConnection(ipAddress), this);
			clientConn->usageMutex.lock();
			if(aio_read(&(clientConn->readingAiocb)) != 0)
			{
				Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Run(): aio_read() failed with \"%s\".\n") % strerror(errno));
			}
			clientConn->usageMutex.unlock();
			clientsMutex.lock();
			if(clients.insert(std::pair<uint64_t, ClientConn*>(clientConn->sessionId, clientConn)).second == false)
			{
				Globals::messenger->write(boost::format("E: ClientsHanler_Aio::Run(): client was NOT inserted under session id = %lld.\n") % clientConn->sessionId);
			}
			clientsMutex.unlock();
			
			//it's not need to free ipAddress - buffer is allocated within inet_ntoa() as "static __thread char buffer[18]"...
		}
	}
	
	isRunnerExited = true;
}

void ClientsHandler_Aio::Send_Single(const uint64_t& sessionId, const void* data, const size_t length)
{
	//Globals::messenger->write(boost::format("I: ClientsHandler_Aio::Send_Single(): locking...\n"));
	clientsMutex.lock();
	//Globals::messenger->write(boost::format("I: ClientsHandler_Aio::Send_Single(): locked.\n"));
	
	std::map<uint64_t, ClientConn*>::iterator it = clients.find(sessionId);
	if(it != clients.end())
	{
		//Globals::messenger->write(boost::format("I: ClientsHandler_Aio::Send_Single(): client found.\n"));
		
		ClientConn* clientConn = it->second;
		
		
		const size_t realDataSize = length + 4;
		void* buffer = malloc(realDataSize);
		*((int32_t*)buffer) = length;
		//cast to char* to mute warning:
		memcpy((void*)((char*)buffer + 4), data, length);
		WritingBufferRef* writingBufferRef = new WritingBufferRef(buffer, 1, realDataSize);
		WritingSival* writingSival = new WritingSival(this, clientConn, writingBufferRef);
		
		clientConn->usageMutex.lock();
		
		//если в данный момент уже что-то отправляется (writingAiocb используется), то нужно добавлять очередную запись в очередь:
		if(clientConn->isWriting)
		{
			clientConn->writingSivals.push_back(writingSival);
		}
		//в противном случае нужно инициировать отправку здесь:
		else
		{
			clientConn->writingAiocb.aio_nbytes = realDataSize;
			clientConn->writingAiocb.aio_buf = buffer;
			clientConn->writingAiocb.aio_sigevent.sigev_value.sival_ptr = (void*)writingSival;
			
			clientConn->isWriting = true;
			
			if(aio_write(&(clientConn->writingAiocb)) != 0)
			{
				Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Send_Single(): aio_write() failed with error \"%s\".\n") % strerror(errno));
			}
		}
		
		clientConn->usageMutex.unlock();
	}
	else
	{
		Globals::messenger->write(boost::format("W: ClientsHandler_Aio::Send_Single(): client under sessiond id = %llu was NOT found.\n") % sessionId);
	}
	
	clientsMutex.unlock();
	
	//Globals::messenger->write(boost::format("I: ClientsHandler_Aio::Send_Single(): end.\n"));
}

void ClientsHandler_Aio::Send_Array(const std::vector<uint64_t> ids, const void* data, const size_t length)
{
	const size_t realDataSize = length + 4;
	void* buffer = malloc(realDataSize);
	*((int32_t*)buffer) = length;
	//cast to char* to mute warning:
	memcpy((void*)((char*)buffer + 4), data, length);
	const size_t idsCount = ids.size();
	WritingBufferRef* writingBufferRef = new WritingBufferRef(buffer, idsCount, realDataSize);
	
	clientsMutex.lock();
	
	for(int i = 0; i < idsCount; i++)
	{
		std::map<uint64_t, ClientConn*>::iterator it = clients.find(ids[i]);
		if(it != clients.end())
		{
			ClientConn* clientConn = it->second;
			
			WritingSival* writingSival = new WritingSival(this, clientConn, writingBufferRef);
			
			clientConn->usageMutex.lock();
			
			//если для этого клиента уже осуществляется запись, то нужно добавлять в очередь:
			if(clientConn->isWriting)
			{
				clientConn->writingSivals.push_back(writingSival);
			}
			//в противном случае нужно инициировать запись:
			else
			{
				clientConn->writingAiocb.aio_nbytes = realDataSize;
				clientConn->writingAiocb.aio_buf = buffer;
				clientConn->writingAiocb.aio_sigevent.sigev_value.sival_ptr = (void*)writingSival;
				
				clientConn->isWriting = true;
				
				if(aio_write(&(clientConn->writingAiocb)) != 0)
				{
					Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Send_Array(): aio_write() failed with error \"%s\".\n") % strerror(errno));
				}
			}
			
			clientConn->usageMutex.unlock();
		}
		else
		{
			writingBufferRef->Decrement();
		}
	}
	
	clientsMutex.unlock();
}

void ClientsHandler_Aio::Send(const Outcoming& outcoming)
{
	Send_Array(outcoming.recipients, outcoming.data.c_str(), outcoming.data.size());
}

void ClientsHandler_Aio::Send(const uint64_t& sessionId, const OutStream& outStream)
{
	Send_Single(sessionId, outStream.GetData().c_str(), outStream.GetData().size());
}

void ClientsHandler_Aio::Send(const std::vector<uint64_t>& ids, const OutStream& outStream)
{
	Send_Array(ids, outStream.GetData().c_str(), outStream.GetData().size());
}

void ClientsHandler_Aio::Send(const uint64_t& sessionId, const std::string& data)
{
	Send_Single(sessionId, data.c_str(), data.size());
}

void ClientsHandler_Aio::Send(const uint64_t& sessionId, const void* data, const size_t length)
{
	Send_Single(sessionId, data, length);
}

void ClientsHandler_Aio::Disconnect_Inner(const uint64_t& id, const bool issueDisconnection)
{
	//Globals::messenger->write(boost::format("I: ClientsHandler_Aio::Disconnect_TU(): removing client under session id = %llu.\n") % id);
	
	clientsMutex.lock();
	
	std::map<uint64_t, ClientConn*>::iterator it = clients.find(id);
	if(it != clients.end())
	{
		ClientConn* clientConn = it->second;
		
		clients.erase(it);
		
		clientsMutex.unlock();
		
		clientConn->usageMutex.lock();
		clientConn->isRemoving = true;
		clientConn->issueDisconnection = issueDisconnection;
		if(shutdown(clientConn->readingSocketDesc, SHUT_RD) != 0)
		{
			Globals::messenger->write(boost::format("E: ClientsHandler_Aio::Disconnect_Inner(): shutdown() failed with error \"%s\".\n") % strerror(errno));
		}
		//соединение будет удалено либо в handle_read() либо в handle_write()...
		clientConn->usageMutex.unlock();
	}
	else
	{
		clientsMutex.unlock();
		
		Globals::messenger->write(boost::format("W: ClientsHandler_Aio::Disconnect_TU(): client under session id = %llu was NOT found.\n") % id);
	}
}

void ClientsHandler_Aio::Disconnect(const uint64_t& id)
{
	Disconnect_Inner(id, false);
}

void ClientsHandler_Aio::Disconnect(const std::vector<uint64_t>& ids)
{
	for(std::vector<uint64_t>::const_iterator it = ids.begin(); it != ids.end(); it++)
	{
		Disconnect_Inner(*it, false);
	}
}

boost::shared_ptr<std::vector<uint64_t> > ClientsHandler_Aio::GetIds()
{
	boost::shared_ptr<std::vector<uint64_t> > ids = boost::make_shared<std::vector<uint64_t> >();
	
	clientsMutex.lock();
	
	for(std::map<uint64_t, ClientConn*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		ids->push_back(it->first);
	}
	
	clientsMutex.unlock();
	
	return ids;
}

void ClientsHandler_Aio::DecrementClientsCount()
{
	clientsCountUsageMutex.lock();
	clientsCount--;
	clientsCountUsageMutex.unlock();
}

}//namespace fos

