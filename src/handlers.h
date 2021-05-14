
/** \file Функции-обработчики пользовательских сообщений. Может использоваться как в случае создания вспомогательного сервера как встроенного в диспетчерский код, так и в случае удалённого оного.*/

#ifndef FOS_HANDLERS_H
#define FOS_HANDLERS_H

#include <string>

namespace fos
{

class Batch;
class ClientTime;
class StartingTime;
class TickTime;
class ExitingTime;

/** Вызывается при старте сервера.
\param startingTime Интерфейс, предоставляющий все доступные операции на этот момент.*/
typedef void(*StartingHandler)(StartingTime* startingTime, void* clientData);

/** Вызывается с указанным при создании сервера интервалом.
\param elapsedSeconds Сколько реально прошло секунд с момента вызова этой функции.*/
typedef void(*TickHandler)(const double elapsedSeconds, TickTime* tickTime);

/** Обработчик самих сообщений, определяющийся конкретной реализацией. Вызывается при получении от клиента сообщения.
\param messageData Данные, полученные от клиента.
\param ClientTime Интерфейс отправки сообщений, получения данных и отправки запросов в базу данных.*/
typedef void(*MessageHandler)(const uint64_t sessionId, const char* messageData, const size_t messageLength, Batch* luggage, ClientTime* clientTime);

/** Обработчик подключений.*/
typedef void(*ConnectionHandler)(const uint64_t sessionId, Batch* luggage, const std::string& ipAddress, ClientTime* clientTime);

/** Обработчик отключений.*/
typedef void(*DisconnectionHandler)(const uint64_t sessionId, Batch* luggage, ClientTime* clientTime);

/** Вызывается в конце остановки работы сервера, когда новые соединения уже не принимаются, но старые соединения ещё поддерживаются. Все данные также ещё присутствуют.*/
typedef void(*ExitingHandler)(ExitingTime* exitingTime);

}//namespace fos

#endif//#ifndef FOS_HANDLERS_H

