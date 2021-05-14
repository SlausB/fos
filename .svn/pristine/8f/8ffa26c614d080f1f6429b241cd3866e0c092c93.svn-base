

/** \file Реализация соединения с клиентами в Linux'е (UNIX) на основе "POSIX Realtime Extension".*/

#if !defined(WINDOWS) && !defined(WIN32) && !defined(WIN64)

#ifndef FOS_CLIENTS_HANDLER_REALTIME_H
#define FOS_CLIENTS_HANDLER_REALTIME_H

#include "client_session.h"

#include <string>
#include <map>

#include <boost/cstdint.hpp>

#include <aio.h>


namespace fos
{


class ClientsConnIFace;
class ClientIFace;
class Outcoming;

/** Сервер для соединения с клиентами.*/
class ClientsHandler_Realtime: public ClientsHandler
{
public:
	
	ClientsHandler_Realtime(const short port, ClientsConnIFace* clientsConnIFace);
	
	/** D->this. Отправка сообщения.*/
	void Send(const Outcoming& outcoming);
	void Send(const uint64_t& sessionId, const OutStream& outStream);
	void Send(const std::vector<uint64_t>& ids, const OutStream& outStream);
	/** D->this. Удаление пользователя с указанным идентификатором.*/
	void Disconnect(const uint64_t& id);
	void Disconnect(const std::vector<uint64_t>& ids);
	/** D->this. Возвращает список текуще подключенных пользователей.*/
	boost::shared_ptr<std::vector<uint64_t> > GetIds();
	
};

}//namespace fos


#endif//#ifndef FOS_CLIENTS_HANDLER_REALTIME_H

#endif

