
#ifndef FOS_STARTING_TIME_H
#define FOS_STARTING_TIME_H

#include <string>

#include "../db/db_iface.h"
#include "../batch_ref.h"

namespace fos
{

class CommonTime;

/** Интерфейс с сервером при вызове StartingHandler.*/
class StartingTime
{
public:
	
#ifndef FOS_NO_DB
	/** Синхронный запрос в базу данных.*/
	virtual db::ResponseRef DBRequest(const char* query) = 0;
	virtual db::ResponseRef DBRequest(const std::string& query) = 0;
	virtual db::ResponseRef DBRequest(const boost::format& query) = 0;
	/** Асинхронный запрос в базу данных.*/
	virtual void DBRequestA(const char* query) = 0;
	virtual void DBRequestA(const std::string& query) = 0;
	virtual void DBRequestA(const boost::format& query) = 0;
	/** Добавить фильтр вывода сообщений об ошибках из базы данных. Если сообщение об ошибке содержит указанную подстроку, то сообщение выводиться не будет.*/
	virtual void DBAddFilter(const char* msg) = 0;
#endif//#ifndef FOS_NO_DB
	
	/** Синхронное получение данных.*/
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
	
	/** Выделяет интерфейс с сервером, который можно использовать где угодно и когда угодно. Должен быть удалён самостоятельно.*/
	virtual CommonTime* AllocateCommonTime() = 0;
	
	/** Returns the raw pointer to the server - try not to use it and only if you know what you are doing.
        It's very sad then it's need to introduce such function to be able implement needed right now feature.*/
        virtual Dispatcher* GetDispatcher() = 0;
};

}//namespace fos

#endif//#ifndef FOS_STARTING_TIME_H

