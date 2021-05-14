

#include "clients_handler_realtime.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <aio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>

#include <map>

#include <boost/cstdint.hpp>


//vast majority of tutorials says that it's need realtime versions of signals to handle asynchronous read/write/accept operations to use realtime benefits (+4 because "LinuxThreads implementation uses the first three real-time signals" - don't risk with the +3):
#define FOS_ACCEPT_SIGNAL (SIGRTMIN + 4)
#define FOS_DATA_SIGNAL (FOS_ACCEPT_SIGNAL + 1)
#define FOS_CUSTOM_SIGNAL (FOS_DATA_SIGNAL + 1)
#define FOS_WRITE_SIGNAL (FOS_CUSTOM_SIGNAL + 1)

ClientsConnIFace* clientsConnIFace;

/** Socket desc which handles incoming connections.*/
int acceptorSocket;

/** Data associated with single client (socket) connection.*/
class ClientConn
{
public:
	
	ClientConn(const int socketDesc, const uint64_t& sessionId): socketDesc(socketDesc), sessionId(sessionId), tempReadBuffer(NULL), currentTempBufferSize(1), currentTempBufferUsage(0), tempBufferStackPointer(0)
	{
		memset(&readingAiocb, 0, sizeof(struct aiocb));
		readingAiocb.aio_nbytes = BUFFER_SIZE;
		readingAiocb.aio_fildes = socketDesc;
		readingAiocb.aio_buf = readBuffer;
		readingAiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
		readingAiocb.aio_sigevent.sigev_signo = FOS_DATA_SIGNAL;
		readingAiocb.aio_sigevent.sigev_value.sival_ptr = (void*)this;
	}
	
	~ClientConn()
	{
		free(tempReadBuffer);
	}
	
	/** Обработать полученные данные, находящиеся в буфере readingAiocb.*/
	void HandleData(const int readBytes)
	{
		//если запасов во временном буфере не хватает для добавления принятых данных:
		if((currentTempBufferSize - currentTempBufferUsage) < readBytes)
		{
			currentTempBufferSize += currentTempBufferSize - currentTempBufferUsage + readBytes;
			tempReadBuffer = (char*)realloc(tempReadBuffer, currentTempBufferSize);
		}
		
		//читаем полученное сообщение во временный буфер:
		memcpy(tempReadBuffer + currentTempBufferUsage, readBuffer, readBytes);
		currentTempBufferUsage += readBytes;
		
		bool shrinkTempBuffer = false;
		
		//если "уже" можно прочитать размер входящего сообщения:
READ_READY_MESSAGE:
		if(currentTempBufferUsage >= sizeof(int32_t))
		{
			//размер входящего сообщения в байтах:
			const int32_t messageSize = *((int32_t*)tempReadBuffer);
			const int realMessageSize = messageSize + sizeof(int32_t);
			//если уже всё сообщение получено:
			if(currentTempBufferUsage >= realMessageSize)
			{
				clientsConnIFace->HandleMessage(sessionId, messageSize, tempReadBuffer + sizeof(int32_t));
				
				//фиксируем очередной размер временного буфера:
				tempBufferStack[tempBufferStackPointer] = currentTempBufferSize;
				tempBufferStackPointer++;
				if(tempBufferStackPointer >= TEMP_BUFFER_RING_SIZE)
				{
					tempBufferStackPointer = 0;
				}
				
				shrinkTempBuffer = true;
				
				//отбрасываем полностью переданное сообщение:
				const int remainedDataSize = currentTempBufferUsage - realMessageSize;
				//если что-то остаётся после отбрасывания:
				if(remainedDataSize > 0)
				{
					memmove(tempReadBuffer, tempReadBuffer + realMessageSize, remainedDataSize);
					currentTempBufferUsage -= realMessageSize;
					//возможно есть ещё уже полученные сообщения:
					goto READ_READY_MESSAGE;
				}
				else
				{
					currentTempBufferUsage = 0;
				}
			}
		}
		
		//ужимаем физический размер временного буфера если он больше среднего размера буфера за последние TEMP_BUFFER_STACK_SIZE и больше текущего использования данных:
		if(shrinkTempBuffer)
		{
			if(currentTempBufferSize > currentTempBufferUsage)
			{
				int64_t buffersSum = 0;
				for(int i = 0; i < TEMP_BUFFER_RING_SIZE; i++)
				{
					buffersSum += tempBufferStack[i];
				}
				const int averageBufferSize = (int)(buffersSum / TEMP_BUFFER_RING_SIZE);
				
				if(averageBufferSize < currentTempBufferSize)
				{
					currentTempBufferSize -= currentTempBufferSize - averageBufferSize;
					if(currentTempBufferSize < currentTempBufferUsage)
					{
						currentTempBufferSize = currentTempBufferUsage;
					}
					//здесь происходит только уменьшение буфера, поэтому память перемещаться не должна, однако GCC жалуется на неиспользование возвращаемого значение (да и мало ли), поэтому используем его:
					tempReadBuffer = (char*)realloc(tempReadBuffer, currentTempBufferSize);
				}
			}
		}
	}
	
	const int socketDesc;
	
	static const int BUFFER_SIZE = 8192;
	/** Куда считываются входящие сообщения.*/
	char readBuffer[BUFFER_SIZE];
	/** Буфер формирования целостных сообщений.*/
	char* tempReadBuffer;
	/** Текущий физический (частично может быть незаполненным) размер временного буфера в байтах.*/
	int currentTempBufferSize;
	/** Количество используемых байтов во временном буфере. Всегда <= currentTempBufferSize .*/
	int currentTempBufferUsage;
	/** Сколько последних чтений рассматривыется для определения максимального размера временного буфера.*/
	static const int TEMP_BUFFER_RING_SIZE = 5;
	/** Для определения максимального размера временного буфера чтения.*/
	int tempBufferStack[TEMP_BUFFER_RING_SIZE];
	int tempBufferStackPointer;
	
	struct aiocb readingAiocb;
	
	/** Идентификатор клиента внутри сервера для связки получаемых и отправляемых сообщений.*/
	uint64_t sessionId;
	
	/** Текуще выполняемые записи в сокет (отправки сообщений). FIFO.*/
	std::queue<aiocb*> writingAiocbs;
};

/** Все текуще подключённые клиенты.*/
std::map<uint64_t, ClientConn*> clients;

class CustomData
{
public:
	CustomData(const int type, void* data): type(type), data(data)
	{
	}
	
	/** Каких типов бывают данные, переданные при отправке пользовательского сигнала.*/
	enum
	{
		SEND,	/**< Асинхронная отправка данных.*/
		DISCONNECT,	/**< Асинхронный разрыв соединения по инициативе сервера.*/
		GET_IDS,	/**< Синхронное получение идентификаторов всех текуще подключённых клиентов.*/
	};
	int type;
	
	/** Данные в зависимости от типа.*/
	void* data;
};

class CustomData_Send
{
public:
	CustomData_Send(const uint64_t& recipient, const char* data, const int length): recipient(recipient), realLength(length + sizeof(int32_t))
	{
		this->data = (char*)malloc(realLength);
		*((int*)this->data) = length;
		memcpy(this->data + sizeof(int32_t), data, length);
		//this->data будет передано в aiocb...
	}
	
	uint64_t recipient;
	char* data;
	int realLength;
};

class CustomData_Disconnect
{
public:
	CustomData_Disconnect(const uint64_t& client): client(client)
	{
	}
	
	uint64_t client;
};

class CustomData_GetIds
{
public:
	CustomData_GetIds(): isDone(false)
	{
	}
	
	boost::shared_ptr<std::vector<uint64_t> > ids;
	bool isDone;
};

/** Accepts incoming connections.*/
void AcceptHandler(int signo, siginfo_t* info, void* context)
{
	const int signalSocket = info->si_fd;
	
	printf("AcceptHandler(%d): si_code = %d, acc = %d, si_fd = %d.\n", signo, info->si_code, acceptorSocket, signalSocket);
	
	//incoming connection:
	if(signalSocket == acceptorSocket)
	{
		//sometimes several connections comes at once and "signal" tutorial says that "Multiple instances of real-time signals can be queued. By contrast, if multiple instances of a standard signal are delivered while that signal is currently blocked, then only one instance is queued.":
		for(;;)
		{
			struct sockaddr_in address;
			socklen_t addressLen = sizeof(struct sockaddr_in);
			const int incomingSocket = accept(signalSocket, (struct sockaddr*)&address, &addressLen);
			if(incomingSocket == -1)
			{
				//it's alright - there are just no incoming connections anymore:
				if(errno == EAGAIN || errno == EWOULDBLOCK)
				{
					break;
				}
				//error:
				else
				{
					Globals::messenger->write(boost::format("E: AcceptHandler(): accept() failed with \"%s\".\n") % strerror(errno));
				}
			}
			else
			{
				//say socket to append outcoming messages at writing:
				const int currentFlags = fcntl(incomingSocket, F_GETFL, 0);
				if(currentFlags < 0)
				{
					Globals::messenger->write(boost::format("E: AcceptHandler(): fcntl(F_GETFL) failed with \"%s\".\n") % strerror(errno));
				}
				if(fcntl(incomingSocket, F_SETFL, currentFlags | O_APPEND | FASYNC) == -1)
				{
					Globals::messenger->write(boost::format("E: AcceptHandler(): fcntl(F_SETFL) failed with \"%s\".\n") % strerror(errno));
				}
				
				char* ipAddress = inet_ntoa(address.sin_addr);
				//Globals::messenger->write(boost::format("I: AcceptHandler(): incoming connection from \"%s\".\n") % ipAddress);
				
				ClientConn* clientConn = new ClientConn(incomingSocket, clientsConnIFace->HandleConnection(ipAddress));
				printf("clientConn->currentTempBufferSize = %d\n", clientConn->currentTempBufferSize);
				printf("sessionId = %lld\n", clientConn->sessionId);
				if(clients.insert(std::pair<uint64_t, ClientConn*>(clientConn->sessionId, clientConn)).second == false)
				{
					Globals::messenger->write(boost::format("E: AcceptHandler(): client was NOT inserted.\n"));
				}
				if(aio_read(&(clientConn->readingAiocb)) != 0)
				{
					Globals::messenger->write(boost::format("E: AcceptHandler(): aio_read() failed with \"%s\".\n") % strerror(errno));
				}
				
				//it's not need to free ipAddress - buffer is allocated within inet_ntoa() as "static __thread char buffer[18]"...
			}
		}
	}
	//incoming data:
	else
	{
		Globals::messenger->write(boost::format("E: AcceptHandler(): invalid socket.\n"));
	}
}

/** Accepts incoming data.*/
void DataHandler(int signo, siginfo_t* info, void* context)
{
	//printf("DataHandler(%d)\n", signo);
	
	ClientConn* clientConn = (ClientConn*)info->si_value.sival_ptr;
	//Globals::messenger->write(boost::format("I: DataHandler(): sessionId = %lld.\n") % clientConn->sessionId);
	
	const int readingErrno = aio_error(&(clientConn->readingAiocb));
	if(readingErrno != 0)
	{
		Globals::messenger->write(boost::format("E: DataHandler(): reading failed with \"%s\".\n") % strerror(readingErrno));
	}
	else
	{
		const int readBytes = aio_return(&(clientConn->readingAiocb));
		//some error:
		if(readBytes < 0)
		{
			Globals::messenger->write(boost::format("E: DataHandler(): read %d bytes.\n") % readBytes);
		}
		//connection closed by peer:
		else if(readBytes == 0)
		{
			if(clients.erase(clientConn->sessionId) != -1)
			{
				Globals::messenger->write(boost::format("E: DataHandler(): client %lld was NOT erased.\n") % clientConn->sessionId);
			}
			delete clientConn;
			clientsConnIFace->HandleDisconnection(clientConn->sessionId);
		}
		//some incoming data:
		else
		{
			clientConn->HandleData(readBytes);
			
			//продолжаем чтение:
			if(aio_read(&clientConn->readingAiocb) != 0)
			{
				Globals::messenger->write(boost::format("E: DataHandler(): aio_read() failed with \"%s\".\n") % strerror(errno));
			}
		}
	}
}

void CustomHandler(int signo, siginfo_t* info, void* context)
{
	//printf("CustomHandler(%d)\n", signo);
	CustomData* customData = (CustomData*)info->si_value.sival_ptr;
	switch(customData->type)
	{
	case CustomData::SEND:
		{
			CustomData_Send* customData_Send = (CustomData_Send*)customData->data;
			
			std::map<uint64_t, ClientConn*>::iterator it = clients.find(customData_Send->recipient);
			if(it == clients.end())
			{
				Globals::messenger->write(boost::format("E: CustomHandler(): send to inexistent (%lld) client.\n") % customData_Send->recipient);
			}
			else
			{
				ClientConn* clientConn = it->second;
				/*aiocb* writingAiocb = new aiocb;
				memset(writingAiocb, 0, sizeof(struct aiocb));
				writingAiocb->aio_nbytes = customData_Send->realLength;
				writingAiocb->aio_fildes = clientConn->socketDesc;
				writingAiocb->aio_buf = customData_Send->data;
				writingAiocb->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
				writingAiocb->aio_sigevent.sigev_signo = FOS_WRITE_SIGNAL;
				writingAiocb->aio_sigevent.sigev_value.sival_ptr = clientConn;
				clientConn->writingAiocbs.push(writingAiocb);
				if(aio_write(writingAiocb) != 0)
				{
					Globals::messenger->write(boost::format("E: CustomHandler(): aio_write() failed with \"%s\".\n") % strerror(errno));
				}
				else
				{
					Globals::messenger->write(boost::format("I: CustomHandler(): aio_write() successfully called with %d bytes.\n") % writingAiocb->aio_nbytes);
				}
				Globals::messenger->write(boost::format("I: aio_error: \"%s\".\n") % strerror(aio_error(writingAiocb)));*/
				write(clientConn->socketDesc, customData_Send->data, customData_Send->realLength);
			}
			
			delete customData_Send;
			delete customData;
		}
		break;
	
	case CustomData::DISCONNECT:
		{
			CustomData_Disconnect* customData_Disconnect = (CustomData_Disconnect*)customData->data;
			
			delete customData_Disconnect;
			delete customData;
		}
		break;
	
	case CustomData::GET_IDS:
		{
			CustomData_GetIds* customData_GetIds = (CustomData_GetIds*)customData->data;
			
			for(std::map<uint64_t, ClientConn*>::const_iterator it = clients.begin(); it != clients.end(); it++)
			{
				customData_GetIds->ids->push_back(it->first);
			}
			customData_GetIds->isDone = true;
			
			//ничего удалять не нужно - объекты используются синхронно из стека функции...
		}
		break;
	}
}

void WriteHandler(int signo, siginfo_t* info, void* context)
{
	printf("WriteHandler(%d)\n", signo);
	ClientConn* clientConn = (ClientConn*)info->si_value.sival_ptr;
	if(clientConn->writingAiocbs.empty())
	{
		Globals::messenger->write(boost::format("E: WriteHandler(): stack is empty.\n"));
	}
	else
	{
		aiocb* writingAiocb = clientConn->writingAiocbs.front();
		clientConn->writingAiocbs.pop();
		const int result = aio_return(writingAiocb);
		if(result != writingAiocb->aio_nbytes)
		{
			Globals::messenger->write(boost::format("E: WriteHandler(): NOT all (%d) bytes written (%d).\n") % writingAiocb->aio_nbytes % result);
		}
		const int errorNumber = aio_error(writingAiocb);
		if(errorNumber != 0)
		{
			Globals::messenger->write(boost::format("E: WriteHandler(): writing failed with \"%s\".\n") % strerror(errno));
		}
		else
		{
			Globals::messenger->write(boost::format("I: WriteHandler(): %d bytes successfully written.\n") % writingAiocb->aio_nbytes);
		}
		free((void*)writingAiocb->aio_buf);
		delete writingAiocb;
	}
}

ClientsHandler_Realtime::ClientsHandler_Realtime(const short port, ClientsConnIFace* clientsConnIFace)
{
	::clientsConnIFace = clientsConnIFace;
	clientsConnIFace->SetClientsHandler(this);
	
	sigset_t globalMask;
	sigemptyset(&globalMask);
	sigaddset(&globalMask, FOS_ACCEPT_SIGNAL);
	sigaddset(&globalMask, FOS_DATA_SIGNAL);
	sigaddset(&globalMask, FOS_CUSTOM_SIGNAL);
	sigaddset(&globalMask, FOS_WRITE_SIGNAL);
	sigprocmask(SIG_BLOCK, &globalMask, NULL);
	
	//catching the "accept" signal:
	struct sigaction acceptSigaction;
	acceptSigaction.sa_sigaction = AcceptHandler;
	if(sigemptyset(&acceptSigaction.sa_mask) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigemptyset(accept) failed with \"%s\".\n") % strerror(errno));
	//mute all other signals to achieve atomicity:
	if(sigaddset(&acceptSigaction.sa_mask, FOS_ACCEPT_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(accept, ACCEPT) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&acceptSigaction.sa_mask, FOS_DATA_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(accept, DATA) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&acceptSigaction.sa_mask, FOS_CUSTOM_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(accept, CUSTOM) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&acceptSigaction.sa_mask, FOS_WRITE_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHander_Realtime::ClientsHandler_Realtime(): sigaddset(accept, WRITE) failed with \"%s\".\n") % strerror(errno));
	acceptSigaction.sa_flags = SA_SIGINFO;
	if(sigaction(FOS_ACCEPT_SIGNAL, &acceptSigaction, NULL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaction(accept) failed with \"%s\".\n") % strerror(errno));
	
	
	//catching the "data" signal:
	struct sigaction dataSigaction;
	dataSigaction.sa_sigaction = DataHandler;
	if(sigemptyset(&dataSigaction.sa_mask) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigemptyset(data) failed with \"%s\".\n") % strerror(errno));
	//mute all other signals to achieve atomicity:
	if(sigaddset(&dataSigaction.sa_mask, FOS_ACCEPT_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(data, ACCEPT) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&dataSigaction.sa_mask, FOS_CUSTOM_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(data, CUSTOM) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&dataSigaction.sa_mask, FOS_WRITE_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(data, WRITE) failed with \"%s\".\n") % strerror(errno));
	dataSigaction.sa_flags = SA_SIGINFO;
	if(sigaction(FOS_DATA_SIGNAL, &dataSigaction, NULL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaction(read) failed with \"%s\".\n") % strerror(errno));
	
	
	//catching custom signals:
	struct sigaction customSigaction;
	customSigaction.sa_sigaction = CustomHandler;
	if(sigemptyset(&customSigaction.sa_mask) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigemptyset(custom) failed with \"%s\".\n") % strerror(errno));
	//mute all other signals to achieve atomicity:
	if(sigaddset(&customSigaction.sa_mask, FOS_ACCEPT_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(custom, ACCEPT) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&customSigaction.sa_mask, FOS_DATA_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(custom, DATA) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&customSigaction.sa_mask, FOS_WRITE_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(custom, WRITE) failed with \"%s\".\n"));
	customSigaction.sa_flags = SA_SIGINFO;
	if(sigaction(FOS_CUSTOM_SIGNAL, &customSigaction, NULL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaction(custom) failed with \"%s\".\n") % strerror(errno));
	

	//catching the "write" signal:
	struct sigaction writeSigaction;
	writeSigaction.sa_sigaction = WriteHandler;
	if(sigemptyset(&writeSigaction.sa_mask) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigemptyset(write) failed with \"%s\".\n") % strerror(errno));
	//mute all other signals to achieve atomicity:
	if(sigaddset(&writeSigaction.sa_mask, FOS_ACCEPT_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(write, ACCEPT) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&writeSigaction.sa_mask, FOS_DATA_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(write, DATA) failed with \"%s\".\n") % strerror(errno));
	if(sigaddset(&writeSigaction.sa_mask, FOS_CUSTOM_SIGNAL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaddset(write, CUSTOM) failed with \"%s\".\n") % strerror(errno));
	writeSigaction.sa_flags = SA_SIGINFO;
	if(sigaction(FOS_WRITE_SIGNAL, &writeSigaction, NULL) != 0) Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): sigaction(write) failed with \"%s\".\n") % strerror(errno));
	
	
	//handle SIGPIPE signals to not to exit in case of work with disconnected sockets because Linux' tutorial says that "A SIGPIPE signal is raised if a process sends or receives on a broken stream; this causes naive processes, which do not handle the signal, to exit.":
	//...
	
	//handle SIGURG signals accordingly manual: "If the file descriptor fd refers to a socket, F_SETOWN also selects the recipient of SIGURG signals that are delivered when out-of-band data arrives on that socket. (SIGURG is sent in any situation where select(2) would report the socket as having an "exceptional condition".)":
	//...
	
	acceptorSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(acceptorSocket == -1)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): socket() failed with \"%s\".\n") % strerror(errno));
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if(bind(acceptorSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): bind() failed with \"%s\".\n") % strerror(errno));
	}
	
	//catching socket signals:
	fcntl(acceptorSocket, F_SETOWN, getpid());
		
	const int currentFlags = fcntl(acceptorSocket, F_GETFL, 0);
	if(currentFlags < 0)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): fcntl(F_GETFL) failed with \"%s\".\n") % strerror(errno));
	}
	//say socket to not to block when accept() will be used at incoming connections handling (O_NONBLOCK) and generate events (FASYNC):
	if(fcntl(acceptorSocket, F_SETFL, currentFlags | O_NONBLOCK | FASYNC) == -1)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): fcntl(F_SETFL) failed with \"%s\".\n") % strerror(errno));
	}
	
	//say socket to generate event at incoming connections:
	if(fcntl(acceptorSocket, F_SETSIG, FOS_ACCEPT_SIGNAL) != 0)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::ClientsHandler_Realtime(): fcntl(F_SETSIG) failed with \"%s\".\n") % strerror(errno));
	}
	
	listen(acceptorSocket, SOMAXCONN);
}

void ClientsHandler_Realtime::Send(const Outcoming& outcoming)
{
	//printf("send outcoming\n");
	CustomData_Send* customData_Send = new CustomData_Send(outcoming.recipients[0], outcoming.data.c_str(), outcoming.data.size());
	union sigval customData;
	customData.sival_ptr = new CustomData(CustomData::SEND, customData_Send);
	if(sigqueue(getpid(), FOS_CUSTOM_SIGNAL, customData) != 0)
	{
		Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::Send(outcoming): sigqueue failed with \"%s\".\n") % strerror(errno));
	}
}

void ClientsHandler_Realtime::Send(const uint64_t& sessionId, const OutStream& outStream)
{
	printf("send outStream(sessionId)\n");
}

void ClientsHandler_Realtime::Send(const std::vector<uint64_t>& ids, const OutStream& outStream)
{
	printf("send outStream(ids)\n");
}

void ClientsHandler_Realtime::Disconnect(const uint64_t& id)
{
	Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::Disconnect(): is NOT implemented yet.\n"));
}

void ClientsHandler_Realtime::Disconnect(const std::vector<uint64_t>& ids)
{
	Globals::messenger->write(boost::format("E: ClientsHandler_Realtime::Disconnect(): is NOT implemented yet.\n"));
}

boost::shared_ptr<std::vector<uint64_t> > ClientsHandler_Realtime::GetIds()
{
	return boost::make_shared<std::vector<uint64_t> >();
}

