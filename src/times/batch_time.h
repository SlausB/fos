
#ifndef FOS_BATCH_TIME_H
#define FOS_BATCH_TIME_H

#include "../db/db_iface.h"

#include <string>

namespace fos
{

class CommonTime;

/** Возможный интерфейс с framework'ом при обработке событий данных.*/
class BatchTime
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
	
	virtual CommonTime* AllocateCommonTime() = 0;
};

}//namespace fos

#endif//#ifndef FOS_BATCH_TIME_H

