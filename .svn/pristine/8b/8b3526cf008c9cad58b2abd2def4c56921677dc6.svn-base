
#ifndef FOS_EXITING_TIME_H
#define FOS_EXITING_TIME_H

namespace fos
{

/** Интерфейс с сервером при его остановке.*/
class ExitingTime
{
public:
	
#ifndef FOS_NO_DB
	/** Синхронный запрос в базу данных.*/
	virtual db::ResponseRef DBRequest(const char* query) = 0;
	virtual db::ResponseRef DBRequest(const std::string& query) = 0;
	virtual db::ResponseRef DBRequest(const boost::format& query) = 0;
	/** Отправка запроса в базу данных без ожидания ответа от неё.*/
	virtual void DBRequestA(const char* query) = 0;
	virtual void DBRequestA(const std::string& query) = 0;
	virtual void DBRequestA(const boost::format& query) = 0;
#endif//#ifndef FOS_NO_DB
	
	/** Отправка сообщения.*/
	virtual void Send(const Outcoming& outcoming) = 0;
	virtual void Send(const uint64_t& sessionId, const OutStream& outStream) = 0;
	virtual void Send(const std::vector<uint64_t>& ids, const OutStream& outStream) = 0;
	
	/** Синхронное получение данных по указанному идентификатору.*/
	virtual BatchRef GetBatch(const BatchId* batchId) = 0;
	/** -> LiteralBatchId */
	virtual BatchRef GetBatch(const char* id) = 0;
	/** -> LiteralBatchId */
	virtual BatchRef GetBatch(const std::string& id) = 0;
	/** -> NumericalBatchId */
	virtual BatchRef GetBatch(const uint64_t& id) = 0;
	
	/** Прекращение использования данных с указанным идентификатором. Если у данных больше нет пользователей, то они будут удалены (вызвано Batch::OnDestruction()). Все данные так-и-так будут удалены почти сразу после вызова ExitingHandler'а.*/
	virtual void DtopBatch(const BatchId* batchId) = 0;
	virtual void DtopBatch(const char* id) = 0;
	virtual void DtopBatch(const std::string& id) = 0;
	virtual void DtopBatch(const uint64_t& id) = 0;
	
	/** Отключить пользователя с указанным идентификатором.*/
	virtual void Disconnect(const uint64_t& id) = 0;
	/** Отключить пользователей с указанными идентификаторами.*/
	virtual void Disconnect(const std::vector<uint64_t>& ids) = 0;
	
	/** Получить идентификаторы всех пользователей, подключённых к серверу в данный момент.*/
	virtual boost::shared_ptr<std::vector<uint64_t> > GetIds() = 0;
};

}//namespace fos

#endif//#ifndef FOS_EXITING_TIME_H

