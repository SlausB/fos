

#ifndef FOS_CLIENT_TIME_H
#define FOS_CLIENT_TIME_H

#include "../outcoming.h"
#include "../stream/stream.h"

#include <boost/cstdint.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace fos
{

class BatchRef;

/** Интерфейс с сервером при обработке сообщений от клиентов (подключение, сообщение, отключение).*/
class ClientTime
{
public:
	
#ifndef FOS_NO_DB
	/** Синхронный запрос в базу данных.*/
	/** Если use = true, то используется переданная область памяти при отправке запроса и освобождается при получении результата запроса.
	\param size Указать если известен размер строки БЕЗ символа конца строки. Если размер неизвестен, то 0.*/
	virtual db::ResponseRef DBRequest( char* query, const bool use, uint32_t size = 0 ) = 0;
	/** Данные для запроса копируются.*/
	virtual db::ResponseRef DBRequest( const char* query ) = 0;
	virtual db::ResponseRef DBRequest( const std::string& query ) = 0;
	virtual db::ResponseRef DBRequest( const boost::format& query ) = 0;
	/** Отправка запроса в базу данных без ожидания ответа от неё.*/
	/** Если use = true, то используется переданная область памяти при отправке запроса и освобождается при получении результата запроса.
	\param size Указать если известен размер строки БЕЗ символа конца строки. Если размер неизвестен, то 0.*/
	virtual void DBRequestA( char* query, const bool use, uint32_t size = 0 ) = 0;
	/** Данные для запроса копируются.*/
	virtual void DBRequestA( const char* query ) = 0;
	virtual void DBRequestA( const std::string& query ) = 0;
	virtual void DBRequestA( const boost::format& query ) = 0;
#endif//#ifndef FOS_NO_DB
	
	/** Отправка сообщения.*/
	virtual void Send(const Outcoming& outcoming) = 0;
	/** Отправка сообщения текущему пользователю.*/
	virtual void Send(const OutStream& outStream) = 0;
	/** Отправка сообщения указанному пользователю.*/
	virtual void Send(const uint64_t& sessionId, const OutStream& outStream) = 0;
	/** Отправка сообщения текущему пользователю.*/
	virtual void Send(const std::string& data) = 0;
	/** Отправка сообщения текущему пользователю.*/
	virtual void Send(const void* data, const size_t length) = 0;
	
	/** Синхронное получение данных по указанному идентификатору.*/
	virtual BatchRef GetBatch(const BatchId* batchId) = 0;
	/** -> LiteralBatchId */
	virtual BatchRef GetBatch(const char* id) = 0;
	/** -> LiteralBatchId */
	virtual BatchRef GetBatch(const std::string& id) = 0;
	/** -> NumericalBatchId */
	virtual BatchRef GetBatch(const uint64_t& id) = 0;
	
	/** Прекращение использования данных с указанным идентификатором. Если у данных больше нет пользователей, то они будут удалены (вызвано Batch::OnDestruction()).*/
	virtual void DropBatch(const BatchId* batchId) = 0;
	virtual void DropBatch(const char* id) = 0;
	virtual void DropBatch(const std::string& id) = 0;
	virtual void DropBatch(const uint64_t& id) = 0;
	/** DropBatch() с указанием клиента вместо текущего.*/
	virtual void DropBatch(const BatchId* batchId, const uint64_t sessionId) = 0;
	virtual void DropBatch(const char* id, const uint64_t sessionId) = 0;
	virtual void DropBatch(const std::string& id, const uint64_t sessionId) = 0;
	virtual void DropBatch(const uint64_t& id, const uint64_t sessionId) = 0;
	
	/** Отключить пользователя с указанным идентификатором.*/
	virtual void Disconnect(const uint64_t& id) = 0;
	/** Отключить пользователей с указанными идентификаторами.*/
	virtual void Disconnect(const std::vector<uint64_t>& ids) = 0;
	/** Отключить текущего пользователя.*/
	virtual void Disconnect() = 0;
	
	/** Получить идентификаторы всех пользователей, подключённых к серверу в данный момент.*/
	virtual boost::shared_ptr<std::vector<uint64_t> > GetIds() = 0;
	
	/** Выделяет интерфейс с сервером, который можно использовать где угодно и когда угодно. Должен быть удалён самостоятельно.*/
	virtual CommonTime* AllocateCommonTime() = 0;
};

}//namespace fos

#endif//#ifndef FOS_CLIENT_TIME_H

