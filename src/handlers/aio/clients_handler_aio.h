
/** \file Реализация соединения с клиентами на основе POSIX функций aio_read/aio_write/... .*/

#ifndef FOS_CLIENTS_HANDLER_AIO_H
#define FOS_CLIENTS_HANDLER_AIO_H

#include <string>
#include <map>

#include <boost/cstdint.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "../clients_handler.h"

#include "../collector/incoming_data_handler.h"

#include <vector>

#include <aio.h>


namespace fos
{


class ClientsConnIFace;
class Outcoming;
class ClientConn;

/** Сервер для соединения с клиентами.*/
class ClientsHandler_Aio: public ClientsHandler
{
	friend void handle_read(union sigval);
	friend void handle_write(union sigval);
	
public:
	
	ClientsHandler_Aio(const int port, ClientsConnIFace* clientsConnIFace);
	
	/** Деструктор. Удаление всех текуще подключённых клиентов (с вызовами соответствуюего обработчика отключения клиента).*/
	~ClientsHandler_Aio();
	
	/** D->this. Отправка сообщения.*/
	void Send(const Outcoming& outcoming);
	void Send(const uint64_t& sessionId, const OutStream& outStream);
	void Send(const std::vector<uint64_t>& ids, const OutStream& outStream);
	void Send(const uint64_t& sessionId, const std::string& data);
	void Send(const uint64_t& sessionId, const void* data, const size_t length);
	/** D->this. Удаление пользователя с указанным идентификатором.*/
	void Disconnect(const uint64_t& id);
	void Disconnect(const std::vector<uint64_t>& ids);
	/** D->this. Возвращает список текуще подключенных пользователей.*/
	boost::shared_ptr<std::vector<uint64_t> > GetIds();
	
	/** Прекращение получения входящих соединений - остановка этого потока, если это происходит в отдельном потоке.*/
	void CancelAcceptingConnections();
	/** Отключение всех текуще подключённых клиентов с сообщениями отключения для диспетчера.*/
	void DisconnectClients();
	
private:
	
	void Run(const int port);
	
	void Send_Single(const uint64_t& sessionId, const void* data, const size_t length);
	void Send_Array(const std::vector<uint64_t> ids, const void* data, const size_t length);
	
	void Disconnect_Inner(const uint64_t& sessionId, const bool issueDisconnection);
	
	void DecrementClientsCount();
	
	
	ClientsConnIFace* clientsConnIFace;
	
	/** Все имеющиеся на данный момент клиенты.*/
	std::map<uint64_t, ClientConn*> clients;

	/** При использовании клиентов.*/
	boost::mutex clientsMutex;
	
	/** При использовании ссылки на данные для записи в сокет.*/
	boost::mutex writingBufferRefMutex;
	
	/** true - сервер в данный момент останавливает свою работу.*/
	bool isStopping;
	/** true - функция Run() корректно завершила свою работу для остановки сервера.*/
	bool isRunnerExited;
	/** ture - функция Run() начала свою работу.*/
	bool isRunning;
	/** Храним его здесь для возможности прерывания получения входящих подключений.*/
	int acceptingSocket;
	/** Для корректной обработки остановки функции Run().*/
	boost::mutex runnerMutex;
	/** Количество текуще подключённых клиентов. В принципе, необходимо лишь для корректной остановки работы сервера.*/
	int clientsCount;
	/** Специально для clientsCount .*/
	boost::mutex clientsCountUsageMutex;
};

}//namespace fos

#endif//#ifndef FOS_CLIENTS_HANDLER_AIO_H

