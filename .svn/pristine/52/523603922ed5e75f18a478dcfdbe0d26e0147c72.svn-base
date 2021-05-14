

#ifndef COMM_H
#define COMM_H

#include "stream/stream.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <boost/thread.hpp>

#include <deque>

#include "../../../src/sync/context_lock.h"

#include "../../../src/output/messenger.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

class Communication
{
public:
	Communication(const std::string& name, Messenger* messenger);
	
	/** Дисконнект.*/
	~Communication();

	/** Синхронное получение сообщения.*/
	std::string Read();

	/** Отправка данных на сервер.*/
	void Write(const std::string& message);
	void Write(const OutStream& outStream);
	
	
private:
	
	void service_runner();

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

	void handle_write(const boost::system::error_code& error, size_t bytes_transferred, boost::shared_ptr<std::string> sentMessage);


	std::string name;

	/** Используется при отправке сообщений на сервер.*/
	boost::mutex writeMutex;
	/** Используется при синхронном получении сообщений от сервера.*/
	boost::mutex readMutex;

	/** Уже считанные асинхронно данные с сервера. Будут изыматься посредством Read().*/
	std::deque<std::string> incomingMessages;

	/** НЕ NULL если на данный момент вызвана синхронная функция Read(), ожидающая получения сообщения от сервера.*/
	ContextLock* readingContextLock;
	
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::socket socket;

	Messenger* messenger;
	
	/** Используется при удалении соединения для ожидания завершения работы io_service.*/
	ContextLock* serviceLock;
	
	static const int BUFFER_SIZE = 4096;
	/** Куда считываются входящие сообщения.*/
	char readBuffer[BUFFER_SIZE];
	/** Буфер формирования целостных сообщений.*/
	char* tempReadBuffer;
	/** Текущий физический (частично может быть незаполненным) размер временного буфера в байтах.*/
	int currentTempBufferSize;
	/** Количество используемых байтов во временном буфере. Всегдя <= currentTempBufferSize .*/
	int currentTempBufferUsage;
	/** Сколько последних чтений рассматривыется для определения максимального размера временного буфера.*/
	static const int TEMP_BUFFER_RING_SIZE = 5;
	/** Для определения максимального размера временного буфера чтения.*/
	int tempBufferStack[TEMP_BUFFER_RING_SIZE];
	int tempBufferStackPointer;
};


#endif//#ifndef COMM_H

