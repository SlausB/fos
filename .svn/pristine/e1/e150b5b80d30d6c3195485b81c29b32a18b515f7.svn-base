
#include "pql.h"

#include "../externs.h"

#include "../timer.h"

//nothrow:
#include <new>

namespace pq_db
{
	
class Response;

/** Один из результатов запроса в базу данных.
Так как это реализация интерфейса, то экземпляры этого класса будут создаваться только внутри этой реализации - не пользоватиелем.*/
class Result: public ::db::Result
{
	friend class ::pq_db::Response;
	friend void Communication(PqlDbConn* pqlDbConn);

public:
	
	bool IsNull(const int rowIndex, const int columnIndex);
	bool IsNull(const int rowIndex, const char* column);
	
	bool GetBool(const int rowIndex, const int columnIndex);
	bool GetBool(const int rowIndex, const char* column);

	/** Прочитать целочисленное значение из результата. В случае какой-либо ошибки возвращает -3.*/
	int32_t GetInt32(const int rowIndex, const int columnIndex);
	int32_t GetInt32(const int rowIndex, const char *column);
	int64_t GetInt64(const int rowIndex, const int columnIndex);
	int64_t GetInt64(const int rowIndex, const char *column);

	/** Прочитать вещественное значение из результата. В случае какой-либо ошибки возвращает -3.*/
	float GetFloat(const int rowIndex, const int columnIndex);
	float GetFloat(const int rowIndex, const char *column);
	double GetDouble(const int rowIndex, const int columnIndex);
	double GetDouble(const int rowIndex, const char *column);

	/** Прочитать строку из результата. В случае какой-либо ошибки возвращает "error".*/
	const char* GetString(const int rowIndex, const int columnIndex);
	const char* GetString(const int rowIndex, const char *column);

	/** Количество строк в ответе.*/
	size_t GetRowsCount();

	/** Количество столбцов в каждой строке.*/
	size_t GetColumnsCount();
	
	/** Печать всех строк результата.*/
	void Print(Messenger *messenger);

private:

	/** Приватный деструктор чтобы клиент не мог удалить результат.*/
	virtual ~Result();

	PGresult *pgResult;
};

/** Ответ от базы данных.*/
class Response: public ::db::Response
{
	friend void Communication(PqlDbConn* pqlDbConn);

public:

	Response();
	
	virtual ~Response();
	
	int GetResultsCount();

	::db::Result* GetResult(int index);

	void Print(Messenger *messenger);

	bool IsOk();

private:
	
	std::vector<Result*> results;

	bool isOk;
};


/** Ошибочный результат.*/
#define ERROR_BINARY -3
#define ERROR_STRING "error"

/** Пустой результат, возвращаемый в случае какой-либо ошибки.*/
class ExceptionResult: public ::db::Result
{
public:
	
	bool IsNull(const int rowIndex, const int columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::IsNull(const int rowIndex, const int columnIndex) usage. Returning false.\n"));
		return false;
	}
	
	bool IsNull(const int rowIndex, const char* columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::IsNull(const int rowIndex, const char* column) usage. Returning false.\n"));
		return false;
	}
	
	bool GetBool(const int rowIndex, const int columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetBool(const int rowIndex, const int columnIndex) usage. Returning false.\n"));
		return false;
	}
	
	bool GetBool(const int rowIndex, const char* column)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetBool(const int rowIndex, const char* column) usage. Returning false.\n"));
		return false;
	}
	
	int32_t GetInt32(const int rowIndex, const int columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetInt32(const int rowIndex, const int columnIndex) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}

	int32_t GetInt32(const int rowIndex, const char *column)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetInt32(const int rowIndex, const char *column) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}

	int64_t GetInt64(const int rowIndex, const int columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetInt64(const int rowIndex, const int columnIndex) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}

	int64_t GetInt64(const int rowIndex, const char *column)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetInt64(const int rowIndex, char *column) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}


	float GetFloat(const int rowIndex, const int columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetFloat(const int rowIndex, const int columnIndex) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}

	float GetFloat(const int rowIndex, const char *column)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetFloat(const int rowIndex, char *column) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}

	double GetDouble(const int rowIndex, const int columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetDouble(const int rowIndex, const int columnIndex) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}

	double GetDouble(const int rowIndex, const char *column)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetDouble(const int rowIndex, const char *column) usage. Returning %d.\n") % ERROR_BINARY);
		return ERROR_BINARY;
	}


	const char* GetString(const int rowIndex, const int columnIndex)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetString(const int rowIndex, const int columnIndex) usage. Returning \"%s\".\n") % ERROR_STRING);
		return ERROR_STRING;
	}

	const char* GetString(const int rowIndex, const char *column)
	{
		Globals::messenger->write(boost::format("E: ExceptionResult::GetString(const int rowIndex, char *column) usage. Returning \"%s\".\n") % ERROR_STRING);
		return ERROR_STRING;
	}

	size_t GetRowsCount()
	{
		Globals::messenger->write("E: ExceptionResult::GetRowsCount() usage. Returning 0.\n");
		return 0;
	}

	size_t GetColumnsCount()
	{
		Globals::messenger->write("E: ExceptionResult::GetColumnsCount() usage. Returning 0.\n");
		return 0;
	}

	void Print(Messenger *messenger)
	{
		messenger->write("W: ExceptionResult::Print(): nothing to print.\n");
	}
};

ExceptionResult *exceptionResult = new ExceptionResult;


/************************************************** Result *****************************************************/

/** Получить индекс поля по его имени.*/
inline int GetColumnIndex(PGresult *pgResult, const char *name)
{
	const int index	= PQfnumber(pgResult, name);
	if(index <= -1)
	{
		Globals::messenger->write(boost::format("E: pq_db::GetColumnIndex(): there is no column with name \"%s\". -1 will be returned.\n") % name);
		return -1;
	}
	return index;
}

/** Проверка индекса строки на правильность в диапазоне. Возвращает 0 если всё нормально.*/
inline int CheckRowRange(PGresult *pgResult, const int row)
{
	if(row < 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::CheckRowRange(): row = %d is wrong. It must be at least positive or zero. Returning 1.\n") % row);
		return -1;
	}
	const int rowsCount	= PQntuples(pgResult);
	if(row >= rowsCount)
	{
		Globals::messenger->write(boost::format("E: pq_db::CheckRowRange(): row = %d is wrong. There are only %d rows.\n") % row % rowsCount);
		return -2;
	}
	return 0;
}

/** Проверка индекса столбца на правильность в диапазоне. Возвращает 0 если всё нормально.*/
inline int CheckColumnRange(PGresult *pgResult, const int column)
{
	if(column < 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::CheckRowRange(): column = %d is wrong. It must be at least positive or zero. Returning -1.\n") % column);
		return -1;
	}
	const int columnsCount	= PQnfields(pgResult);
	if(column >= columnsCount)
	{
		Globals::messenger->write(boost::format("E: pq_db::CheckRowRange(): column = %d is wrong. There are only %d columns.\n") % column % columnsCount);
		return -2;
	}
	return 0;
}

/** Возвращает true если данные в указанном столбце являются бинарными. В противном случае жалуется об ошибке и возвращает false.*/
inline bool CheckIsBinary(PGresult *pgResult, const int column)
{
	if(PQfformat(pgResult, column) != 1)
	{
		Globals::messenger->write(boost::format("E: pq_db::CheckIsBinary(): column %d is NOT binary. Returning false.\n") % column);
		return false;
	}
	return true;
}

/** Возвращает true если данные в указанном столбце являются бинарными. В противном случае жалуется об ошибке и возвращает false.*/
inline bool CheckIsText(PGresult *pgResult, const int column)
{
	if(PQfformat(pgResult, column) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::CheckIsText(): column %d is NOT binary. Returning false.\n") % column);
		return false;
	}
	return true;
}



bool Result::IsNull(const int rowIndex, const int columnIndex)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::IsNull(%d, %d) failed with row range error. Returning false.\n") % rowIndex % columnIndex);
		return false;
	}
	if(CheckColumnRange(pgResult, columnIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::IsNull(%d, %d) failed with column range error. Returning false.\n") % rowIndex % columnIndex);
		return false;
	}
	
	return PQgetisnull(pgResult, rowIndex, columnIndex) == 1;
}

bool Result::IsNull(const int rowIndex, const char* column)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::IsNull(%d, \"%s\") failed with row range error. Returning false.\n") % rowIndex % column);
		return false;
	}
	const int columnIndex = GetColumnIndex(pgResult, column);
	if(columnIndex < 0)
	{
		Globals::messenger->write(boost::format("pq_db::Result::IsNull(%d, \"%s\") failed with column range error. Returning false.\n") % rowIndex % column);
		return false;
	}
	
	return PQgetisnull(pgResult, rowIndex, columnIndex) == 1;
}

bool Result::GetBool(const int rowIndex, const int columnIndex)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetBool(%d, %d) failed with row range error. Returning false.\n") % rowIndex % columnIndex);
		return false;
	}
	if(CheckColumnRange(pgResult, columnIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetBool(%d, %d) failed with column range error. Returning false.\n") % rowIndex % columnIndex);
		return false;
	}
	
	return strcmp(PQgetvalue(pgResult, rowIndex, columnIndex), "t") == 0;
}

bool Result::GetBool(const int rowIndex, const char* column)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pg_db::Result::GetBool(%d, \"%s\") failed with row range error. Returning false.\n") % rowIndex % column);
		return false;
	}
	const int columnIndex = GetColumnIndex(pgResult, column);
	if(columnIndex < 0)
	{
		Globals::messenger->write(boost::format("pg_db::Result::GetBool(%d, \"%s\") failed with column range error. Returning false.\n") % rowIndex % column);
	}
	
	return strcmp(PQgetvalue(pgResult, rowIndex, columnIndex), "t") == 0;
}

int32_t Result::GetInt32(const int rowIndex, const int columnIndex)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt32(%d, %d) failed with row range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}
	if(CheckColumnRange(pgResult, columnIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt32(%d, %d) failed with column range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return atoi(PQgetvalue(pgResult, rowIndex, columnIndex));
}

int32_t Result::GetInt32(const int rowIndex, const char *column)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt32(%d, \"%s\") failed with row range error. Returning %d.\n") % rowIndex % column % ERROR_BINARY);
		return ERROR_BINARY;
	}
	const int columnIndex = GetColumnIndex(pgResult, column);
	if(columnIndex < 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt32(%d, \"%s\") failed with column range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return atoi(PQgetvalue(pgResult, rowIndex, columnIndex));
}

int64_t Result::GetInt64(const int rowIndex, const int columnIndex)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt64(%d, %d) failed with row range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}
	if(CheckColumnRange(pgResult, columnIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt64(%d, %d) failed with column range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return atol(PQgetvalue(pgResult, rowIndex, columnIndex));
}

int64_t Result::GetInt64(const int rowIndex, const char *column)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt64(%d, \"%s\") failed with row range error. Returning %d.\n") % rowIndex % column % ERROR_BINARY);
		return ERROR_BINARY;
	}
	const int columnIndex = GetColumnIndex(pgResult, column);
	if(columnIndex < 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetInt64(%d, \"%s\") failed with column range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return atol(PQgetvalue(pgResult, rowIndex, columnIndex));
}


float Result::GetFloat(const int rowIndex, const int columnIndex)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetFloat(%d, %d) failed with row range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}
	if(CheckColumnRange(pgResult, columnIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetFloat(%d, %d) failed with column range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return (float)atof(PQgetvalue(pgResult, rowIndex, columnIndex));
}

float Result::GetFloat(const int rowIndex, const char *column)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetFloat(%d, \"%s\") failed with row range error. Returning %d.\n") % rowIndex % column % ERROR_BINARY);
		return ERROR_BINARY;
	}
	const int columnIndex = GetColumnIndex(pgResult, column);
	if(columnIndex < 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetFloat(%d, \"%s\") failed with column range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return (float)atof(PQgetvalue(pgResult, rowIndex, columnIndex));
}

double Result::GetDouble(const int rowIndex, const int columnIndex)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetDouble(%d, %d) failed with row range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}
	if(CheckColumnRange(pgResult, columnIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetDouble(%d, %d) failed with column range error. Returning %d.\n") % rowIndex % columnIndex % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return atof(PQgetvalue(pgResult, rowIndex, columnIndex));
}

double Result::GetDouble(const int rowIndex, const char *column)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetDouble(%d, \"%s\") failed with row range error. Returning %d.\n") % rowIndex % column % ERROR_BINARY);
		return ERROR_BINARY;
	}
	const int columnIndex = GetColumnIndex(pgResult, column);
	if(columnIndex < 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetDouble(%d, \"%s\") failed with column range error. Returning %d.\n") % rowIndex % column % ERROR_BINARY);
		return ERROR_BINARY;
	}

	return atof(PQgetvalue(pgResult, rowIndex, columnIndex));
}


const char* Result::GetString(const int rowIndex, const int columnIndex)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetString(%d, %d) failed with row range error. Returning \"%s\".\n") % rowIndex % columnIndex % ERROR_STRING);
		return ERROR_STRING;
	}
	if(CheckColumnRange(pgResult, columnIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetString(%d, %d) failed with column range error. Returning \"%s\".\n") % rowIndex % columnIndex % ERROR_STRING);
		return ERROR_STRING;
	}

	return PQgetvalue(pgResult, rowIndex, columnIndex);
}

const char* Result::GetString(const int rowIndex, const char *column)
{
	if(CheckRowRange(pgResult, rowIndex) != 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetDouble(%d, \"%s\") failed with row range error. Returning \"%s\".\n") % rowIndex % column % ERROR_STRING);
		return ERROR_STRING;
	}
	const int columnIndex = GetColumnIndex(pgResult, column);
	if(columnIndex < 0)
	{
		Globals::messenger->write(boost::format("E: pq_db::Result::GetDouble(%d, \"%s\") failed with column range error. Returning \"%s\".\n") % rowIndex % column % ERROR_STRING);
		return ERROR_STRING;
	}
	
	return PQgetvalue(pgResult, rowIndex, columnIndex);
}

size_t Result::GetRowsCount()
{
	return PQntuples(pgResult);
}

size_t Result::GetColumnsCount()
{
	return PQnfields(pgResult);
}

Result::~Result()
{
	PQclear(pgResult);
}

void Result::Print(Messenger *messenger)
{
	const int rowsCount	= PQntuples(pgResult);
	const int columnsCount	= PQnfields(pgResult);
	messenger->write(boost::format("%d rows and %d columns:\n") % rowsCount % columnsCount);
	messenger->ii();
	for(int row = 0; row < rowsCount; row++)
	{
		for(int column = 0; column < columnsCount; column++)
		{
			const char *fieldName	= PQfname(pgResult, column);

			const int format	= PQfformat(pgResult, column);
			if(format == 0)
			{
				messenger->write(boost::format("\"%s\": text: \"%s\"\n") % fieldName % PQgetvalue(pgResult, row, column));
			}
			else if(format == 1)
			{
				const int length	= PQgetlength(pgResult, row, column);
				int readingLength	= length;
				if(readingLength > sizeof(int32_t))
				{
					readingLength	= sizeof(int32_t);
				}
				int32_t value;
				memcpy(&value, PQgetvalue(pgResult, row, column), readingLength);
				messenger->write(boost::format("\"%s\": binary of length %d: %d\n") % fieldName % length % value);
			}
			else
			{
				messenger->write(boost::format("\"%s\": format = %d is indefined. Nothing will be printed.\n") % fieldName % format);
			}
		}

		if(row < (rowsCount - 1))
		{
			messenger->write("--------\n");
		}
	}
	messenger->di();
}


/************************************************** Response ************************************************/

int Response::GetResultsCount()
{
	return results.size();
}

::db::Result* Response::GetResult(int index)
{
	if(index < 0)
	{
		Globals::messenger->write(boost::format("E: Response::GetResult(): index = %d is invalid. It must be positive at least (or zero). Exception result will be returned.\n") % index);
		return exceptionResult;
	}
	if(index >= (int)results.size())
	{
		Globals::messenger->write(boost::format("E: Response::GetResult(): index = %d is more then results count = %d. Exception result will be returned.\n") % index % (int)results.size());
		return exceptionResult;
	}

	return results[index];
}

Response::Response(): isOk(true)
{
}

Response::~Response()
{
	for(int i = 0; i < (int)results.size(); i++)
	{
		delete results[i];
	}
}

void Response::Print(Messenger *messenger)
{
	const int resultsCount	= (int)results.size();
	messenger->write(boost::format("PostgreSQL response with %d results:\n") % resultsCount);
	messenger->ii();
	for(int i = 0; i < resultsCount; i++)
	{
		results[i]->Print(messenger);
		if(resultsCount < (i - 1))
		{
			messenger->write("-------------------\n");
		}
	}
	messenger->di();
}

bool Response::IsOk()
{
	return isOk;
}



/************************************************** Communication ************************************************/

/** Отправка запросов в базу данных и получение ответов.*/
void Communication( PqlDbConn* pqlDbConn )
{
	pqlDbConn->stoppingMutex.lock();
	if ( pqlDbConn->isStopping )
	{
		pqlDbConn->stoppingMutex.unlock();
		return;
	}
	pqlDbConn->isCommunicating = true;
	pqlDbConn->stoppingMutex.unlock();
	
	//если за время подключения к базе данных были добавлены какие-либо запросы:
	//ждём доступа к чтению предыдуще добавленных запросов:
	pqlDbConn->queriesMutex.lock();
	const int size = pqlDbConn->queries.size();
	pqlDbConn->queriesMutex.unlock();
	if ( size > 0 )
		//сразу переходим к запросам:
		goto POP_QUERY;
	//изначально нет ни одного запроса, поэтому сразу замораживаем этот поток:
	else
		goto SUSPEND;

	//если мьютекс кто-то освободил (это может сделать только SendQuery()), то появился новый запрос, который нужно отправить в базу данных...

POP_QUERY:
	{
		//ждём доступа к чтению предыдуще добавленных запросов:
		pqlDbConn->queriesMutex.lock();
		//отправляем в БД очередной запрос (если поток разблокирован, то хотя бы один запрос однозначно был добавлен):
		PqlDbConn::Query *query = pqlDbConn->queries[0];
		//pqlDbConn->messenger->write(boost::format("I: sending query \"%s\" ...\n") % query->queryString);
		Timer requestTimer;
		//отправляем очередной ожидающий запрос:
		if ( PQsendQuery( pqlDbConn->conn, query->queryString ) != 1 )
			pqlDbConn->messenger->write( boost::format( "E: PQsendQuery() failed with error \"%s\".\n" ) % PQerrorMessage(pqlDbConn->conn) );
		//открываем доступ к добавлению запросов пока ожидается ответ от базы данных:
		pqlDbConn->queriesMutex.unlock();
		
		
		///получение результата запроса...
		
		
		const bool handlerSettled = query->callback != NULL;
		
		//где будут сформированы все результаты запросов в базу данных:
		Response *response;
		if ( handlerSettled )
		{
			response = new( std::nothrow ) Response;
			if ( response == NULL )
				Globals::messenger->write( boost::format( "E: PostgreSQL: response creation failed with insufficient memory. Reponses will be fetched but will not be returned to the client.\n" ) );
		}
		
		//получение результатов всех запросов (в том числе и тех, на которых нет возвращаемых данных):
FOR_EACH_RESULT:
		PGresult *pgResult = PQgetResult( pqlDbConn->conn );
		if ( pgResult != NULL )
		{
			const ExecStatusType execStatusType = PQresultStatus( pgResult );
			if ( execStatusType == PGRES_FATAL_ERROR )
			{
				const char* errorMsg = PQresultErrorMessage( pgResult );
				if ( pqlDbConn->PassFilter( errorMsg ) )
				{
					pqlDbConn->messenger->write( boost::format( "E: PostgreSQL request failed with \"%s\":\n" ) % PQresStatus( execStatusType ) );
					pqlDbConn->messenger->set_type( Output::OUTPUT_ERROR );
					pqlDbConn->messenger->write( errorMsg );
					pqlDbConn->messenger->write( "	query string:\n" );
					pqlDbConn->messenger->write( boost::format( "	%s\n" ) % query->queryString );
					pqlDbConn->messenger->reset_type();
				}
				
				if ( handlerSettled )
				{
					if ( response != NULL )
						response->isOk = false;
				}
			}
			
			if ( handlerSettled )
			{
				if ( response != NULL )
				{
					Result *result = new( std::nothrow ) Result;
					if ( result == NULL )
					{
						Globals::messenger->write( boost::format( "E: PostgreSQL: result creation failed with insufficient memory. It will not be added.\n" ) );
					}
					else
					{
						result->pgResult = pgResult;
						response->results.push_back( result );
					}
				}
			}
			else
			{
				PQclear( pgResult );
			}
			
			goto FOR_EACH_RESULT;
		}
		
		const double requestSeconds = requestTimer.getElapsedSeconds();
		pqlDbConn->statsMutex.lock();
		pqlDbConn->requestsCount++;
		pqlDbConn->grossSeconds += requestSeconds;
		if ( requestSeconds > pqlDbConn->longestRequestSeconds )
		{
			pqlDbConn->longestRequestSeconds = requestSeconds;
			pqlDbConn->longestRequest = query->queryString;
		}
		if ( requestSeconds < pqlDbConn->fastestRequestSeconds )
		{
			pqlDbConn->fastestRequestSeconds = requestSeconds;
			pqlDbConn->fastestRequest = query->queryString;
		}
		pqlDbConn->statsMutex.unlock();
		
		//здесь используется тот же query, что и был получен для отправки текста запроса в базу данных...
		
		//здесь нельзя закрывать мьютекс запросов, так как в callback'е может (и действительно) вызываться SendQuery()...
		
		//если пользователь задал функцию обратного вызова:
		if ( handlerSettled )
			query->callback( db::ResponseRef( response ), query->parameter );
		
		//результат этого запроса уже получен - он однозначно больше не нужен:
		delete query;
		
		//ждём доступа к чтению предыдуще добавленных запросов:
		pqlDbConn->queriesMutex.lock();
		//он отправлен в базу данных (и результат вот сейчас получен) - выбрасываем его:
		pqlDbConn->queries.pop_front();
		const bool queriesExist	= pqlDbConn->queries.size() > 0;
		pqlDbConn->queriesMutex.unlock();
		
		//если есть запросы к базе данных уже сейчас, то переходим к ним сразу:
		if ( queriesExist )
			//переходим к ожиданию ответа от базы данных:
			goto POP_QUERY;
	}
	
	//замораживаем сами себя (этот же поток) до тех пор, пока не будут добавлены новые запросы (где поток и будет "оживлён"):
SUSPEND:
	{
		boost::mutex::scoped_lock lock( pqlDbConn->commLockMutex );
WAIT:
		pqlDbConn->queriesMutex.lock();
		const int size	= pqlDbConn->queries.size();
		pqlDbConn->queriesMutex.unlock();
		if ( size <= 0 )
		{
			pqlDbConn->thereAreQueries.wait( lock );
			if ( pqlDbConn->isStopping )
			{
				pqlDbConn->isCommunicationExited = true;
				return;
			}
			goto WAIT;
		}
		
		goto POP_QUERY;
	}
}

void PqlDbConn::SendQuery( char* queryText, ::db::Callback callback, void *parameter )
{
	//messenger->printf("I: appending query \"%s\".\n", queryText);
	
	//конструируем запрос здесь чтобы по-меньше времени занимать доступ к чтению/записи запросов:
	Query *query = new Query( queryText, callback, parameter );
	
	boost::mutex::scoped_lock lock( commLockMutex );
	
	//ждём доступа к добавлению запросов:
	queriesMutex.lock();
	
	//добавляем запрос в очередь:
	queries.push_back(query);
	
	//особождаем доступ к чтению запросов:
	queriesMutex.unlock();
	
	//оповещаем процесс общения с базой данных о добавленном сообщении, если он ждёт, конечно:
	thereAreQueries.notify_one();
}

PqlDbConn::PqlDbConn(Messenger *messenger): isStopping(false), isCommunicationExited(false), requestsCount(0), grossSeconds(0), longestRequestSeconds(0), fastestRequestSeconds(999999)
{
	this->messenger	= messenger;
}

PqlDbConn::~PqlDbConn()
{
	//ждём когда будут обработаны все запросы в базу данных:
	messenger->write(boost::format("I: PqlDbConn::~PqlDbConn(): waiting for all queries handling...\n"));
	for( ;; )
	{
		queriesMutex.lock();
		if( queries.empty() )
		{
			queriesMutex.unlock();
			break;
		}
		queriesMutex.unlock();
		boost::this_thread::sleep( boost::posix_time::milliseconds( 10 ) );
	}
	messenger->write( boost::format( "I: PqlDbConn::~PqlDbConn(): all queries successfully handled.\n" ) );
	
	isStopping = true;
	
	//останавливаем поток функции Communication() если она вообще работает:
	messenger->write( boost::format( "I: PqlDbConn::~PqlDbConn(): waiting for Communication() thread to exit...\n" ) );
	stoppingMutex.lock();
	if ( isCommunicating )
	{
		stoppingMutex.unlock();
		while( !isCommunicationExited )
		{
			//пробуждаем поток функции Commication() чтобы у неё была возможность прочитать факт удаления соединения из isStopping:
			thereAreQueries.notify_one();
			
			boost::this_thread::sleep( boost::posix_time::milliseconds( 10 ) );
		}
	}
	else
	{
		stoppingMutex.unlock();
	}
	messenger->write( boost::format( "I: PqlDbConn::~PqlDbConn(): communication thread successfully exited.\n" ) );
	
	/* close the connection to the database and cleanup */
	PQfinish(conn);
}

int PqlDbConn::Connect(const char* dbName, const char* login, const char* password)
{
	/*
	* begin, by setting the parameters for a backend connection if the
	* parameters are null, then the system will try to use reasonable
	* defaults by looking up environment variables or, failing that,
	* using hardwired constants
	*/
	char* pghost	= NULL;              /* host name of the backend server */
	char* pgport	= NULL;              /* port of the backend server */
	char* pgoptions	= NULL;           /* special options to start up the backend server */
	char* pgtty	= NULL;               /* debugging tty for the backend server */

	/* make a connection to the database */
	//conn = PQsetdb(pghost, pgport, pgoptions, pgtty, dbName);
	conn = PQsetdbLogin(pghost, pgport, pgoptions, pgtty, dbName, login, password);

	/*
	* check to see that the backend connection was successfully made
	*/
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		messenger->write(boost::format("E: Connection to database '%s' failed with error \"%s\".\n") % dbName % PQerrorMessage(conn));
		return 1;
	}
	else
	{
		messenger->write("I: PostgreSQL database connected successfully.\n");
	}

	//запускаем поток общения с базой данных:
	try
	{
		boost::thread commThread(Communication, this);
	}
	catch(std::exception e)
	{
		messenger->write(boost::format("E: communication thread creation failed with \"%s\".\n") % e.what());
	}

	return 0;
}

void PqlDbConn::Exit()
{
	PQfinish(conn);
}




PqlDbConn::Query::Query( char* queryString, db::Callback callback, void* parameter ): queryString( queryString ), callback( callback ), parameter( parameter )
{
}

PqlDbConn::Query::~Query()
{
	free( queryString );
}


}//namespace pq_db

