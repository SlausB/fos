

#ifndef FOS_CLIENTS_HANDLER_H
#define FOS_CLIENTS_HANDLER_H

#include <string>
#include <map>

#include <boost/cstdint.hpp>

#include "../meters_factory.h"


namespace fos
{


class ClientsConnIFace;
class ClientIFace;
class Outcoming;

/** Сервер для соединения с клиентами.*/
class ClientsHandler
{
public:
	
	/** Чтобы вызывался деструктор потомков.*/
	virtual ~ClientsHandler();
	
	/** Отправка сообщения.*/
	virtual void Send(const Outcoming& outcoming) = 0;
	virtual void Send(const uint64_t& sessionId, const OutStream& outStream) = 0;
	virtual void Send(const std::vector<uint64_t>& ids, const OutStream& outStream) = 0;
	virtual void Send(const uint64_t& sessionId, const std::string& data) = 0;
	virtual void Send(const uint64_t& sessionId, const void* data, const size_t length) = 0;
	/** Удаление пользователя с указанным идентификатором.*/
	virtual void Disconnect(const uint64_t& id) = 0;
	virtual void Disconnect(const std::vector<uint64_t>& ids) = 0;
	/** Возвращает список текуще подключенных пользователей.*/
	virtual boost::shared_ptr<std::vector<uint64_t> > GetIds() = 0;
	
	/** Прекращение получения входящих соединений - остановка этого потока, если это происходит в отдельном потоке.*/
	virtual void CancelAcceptingConnections() = 0;
	/** Отключение всех текуще подключённых клиентов с сообщениями отключения для диспетчера.*/
	virtual void DisconnectClients() = 0;
	
};

inline ClientsHandler::~ClientsHandler()
{
}

}//namespace fos


#endif//#ifndef FOS_CLIENTS_HANDLER_H

