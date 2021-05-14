

#include "comm.h"

#include "utils.h"

using boost::asio::ip::tcp;

#define ADDRESS "localhost"
//#define ADDRESS "192.168.0.110"
#define PORT "4570"

Communication::Communication(const std::string& name, Messenger* messenger): name(name), messenger(messenger), readingContextLock(NULL), socket(io_service), serviceLock(new ContextLock), tempReadBuffer(NULL), currentTempBufferSize(0), currentTempBufferUsage(0), tempBufferStackPointer(0)
{
	try
	{
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(ADDRESS, PORT);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}

		if (error)
		{
			throw boost::system::system_error(error);
		}
		else
		{
			//messenger.write(boost::format("I: \"%s\" successfully connected to \"%s\".\n") % name % ADDRESS);

			boost::asio::socket_base::send_buffer_size option;
			socket.get_option(option);
			int size = option.value();
			
			memset(tempBufferStack, 0, sizeof(int) * TEMP_BUFFER_RING_SIZE);
			
			socket.async_read_some(boost::asio::buffer(readBuffer, BUFFER_SIZE), boost::bind(&Communication::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			
			boost::thread serviceThread(&Communication::service_runner, this);
		}
	}
	catch (std::exception& e)
	{
		messenger->error(e.what());
		messenger->endl();
	}
}

Communication::~Communication()
{
	//об ожидании завершения асинхронных чтений-записей приходится полагаться на уверение boost.asio "Any asynchronous send, receive or connect operations will be cancelled immediately, and will complete with the boost::asio::error::operation_aborted error.":
	boost::system::error_code ec;
	socket.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both, ec);
	if(ec.value() != 0)
	{
		messenger->write(boost::format("E: ClientSession::~ClientSession(): socket shutdouwn failed with \"%s\".\n") % ec.message());
	}
	socket.close(ec);
	if(ec.value() != 0)
	{
		messenger->write(boost::format("E: ClientSession::~ClientSession(): socket close failed with \"%s\".\n") % ec.message());
	}
	io_service.stop();
	
	serviceLock->Stop();

	free(tempReadBuffer);
}

std::string Communication::Read()
{
	readMutex.lock();

	if(incomingMessages.size() > 0)
	{
		std::string readyMessage = incomingMessages.front();
		incomingMessages.pop_front();
		readMutex.unlock();
		return readyMessage;
	}
	else
	{
		readingContextLock = new ContextLock;

		readMutex.unlock();
		readingContextLock->Stop();
		readMutex.lock();

		readingContextLock = NULL;

		std::string readyMessage = incomingMessages.front();
		incomingMessages.pop_front();
		readMutex.unlock();
		return readyMessage;
	}

	readMutex.unlock();
}

void Communication::Write(const std::string& message)
{
	writeMutex.lock();

	boost::shared_ptr<std::string> sendingMessage = boost::make_shared<std::string>();
	const int size = message.size();
	sendingMessage->append((char*)&size, sizeof(int32_t));
	sendingMessage->append(message);
	boost::asio::async_write(socket, boost::asio::buffer(*sendingMessage), boost::bind(&Communication::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, sendingMessage));
	
	writeMutex.unlock();
}

void Communication::Write(const OutStream& outStream)
{
	Write(outStream.GetData());
}

void Communication::service_runner()
{
	io_service.run();
	
	serviceLock->Resume();
}

void Communication::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		//если запасов во временном буфере не хватает для добавления принятых данных:
		if((currentTempBufferSize - currentTempBufferUsage) < bytes_transferred)
		{
			currentTempBufferSize += currentTempBufferSize - currentTempBufferUsage + bytes_transferred;
			tempReadBuffer = (char*)realloc(tempReadBuffer, currentTempBufferSize);
		}

		//читаем полученное сообщение во временный буфер:
		memcpy(tempReadBuffer + currentTempBufferUsage, readBuffer, bytes_transferred);
		currentTempBufferUsage += bytes_transferred;

		bool shringTempBuffer = false;

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
				//передаём полученное сообщение:
				readMutex.lock();
				incomingMessages.push_back(std::string(tempReadBuffer + sizeof(int32_t), messageSize));
				if(readingContextLock != NULL)
				{
					readingContextLock->Resume();
					readingContextLock = NULL;
				}
				readMutex.unlock();

				//фиксируем очередной размер временного буфера:
				tempBufferStack[tempBufferStackPointer] = currentTempBufferSize;
				tempBufferStackPointer++;
				if(tempBufferStackPointer >= TEMP_BUFFER_RING_SIZE)
				{
					tempBufferStackPointer = 0;
				}

				shringTempBuffer = true;

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
		if(shringTempBuffer)
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
		
		//продолжаем чтение:
		socket.async_read_some(boost::asio::buffer(readBuffer, BUFFER_SIZE), boost::bind(&Communication::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		messenger->write(boost::format("W: reading failed for client \"%s\". Possible server-side disconnection.\n") % name);
	}
}

void Communication::handle_write(const boost::system::error_code& error, size_t bytes_transferred, boost::shared_ptr<std::string> sentMessage)
{
	if(!error)
	{
		const int bytesSent = (*sentMessage).size();
		if(bytesSent != bytes_transferred)
		{
			messenger->write(boost::format("E: sent %d bytes but was transferred just %d for client \"%s\".\n") % bytesSent % bytes_transferred % name);
		}
	}
	else
	{
		messenger->write(boost::format("E: writing failed for client \"%s\".\n") % name);
	}
}
