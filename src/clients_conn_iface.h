
#ifndef FOS_CLIENTS_CONN_IFACE_H
#define FOS_CLIENTS_CONN_IFACE_H

#include <boost/cstdint.hpp>

#include <string>

namespace fos
{

class Dispatcher;
class Outcoming;
class ClientsHandler;

/** Взаимодействие между диспетчером и соединением с клиентами.*/
class ClientsConnIFace
{
public:
	
	ClientsConnIFace(Dispatcher* dispatcher);
	
	~ClientsConnIFace();
	
	void SetClientsHandler(ClientsHandler* clientsHandler);
	
	/** Net->this. Новое соединение.*/
	const uint64_t& HandleConnection(const std::string& ipAddress);
	const uint64_t& HandleConnection(const char* ipAddress);
	
	/** Net->this. Получение сообщения от клиента.*/
	void HandleMessage(const uint64_t& sessionId, const size_t dataLength, const char* messageData);
	
	/** Net->this. Отключение клиента с указанным идентификатором сессии.*/
	void HandleDisconnection(const uint64_t& sessionId);
	
	
	/** D->this. Отправить сообщение.*/
	void Send(const Outcoming& outcoming);
	void Send(const uint64_t& id, const OutStream& outStream);
	void Send(const std::vector<uint64_t>& ids, const OutStream& outStream);
	void Send(const uint64_t& id, const std::string& data);
	void Send(const uint64_t& id, const void* data, const size_t length);
	
	/** D->this. Удаление (отключение) пользователя с указанным идентификатором.*/
	void Disconnect(const uint64_t& id);
	void Disconnect(const std::vector<uint64_t>& ids);
	
	/** D->this. Возвращает список всех подключенных в данный момент клиентов.*/
	boost::shared_ptr<std::vector<uint64_t> > GetIds();
	
	
	/** Прекращение получения входящих соединений - остановка этого потока, если это происходит в отдельном потоке.*/
	void CancelAcceptingConnections();
	/** Отключение всех текуще подключённых клиентов с сообщениями отключения для диспетчера.*/
	void DisconnectClients();
	
private:
	
	Dispatcher* dispatcher;
	
	ClientsHandler* clientsHandler;
};

}//namespace fos

#endif//#ifndef FOS_CLIENTS_CONN_IFACE_H

