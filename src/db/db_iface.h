
/** Интерфейс к произвольной базе данных. Абстрактный класс, который реализуется для каждого типа базы данных.*/

#ifndef DB_INTERFACE_H
#define DB_INTERFACE_H

//vc2008 does not have <stdint.h>:
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#include <boost/cstdint.hpp>
using boost::int32_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;
#else
#include <stdint.h>
#endif

#include "../output/messenger.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "boost/thread.hpp"

#include <vector>
#include <string>


namespace db
{

class Response;

/** Проверяет достаточность параметров и пишет сообщение об ошибке с указанным контекстом. Возвращает 0 если всё в порядке.
\param messenger Куда выводить сообщение.
\param response Где проверять количество параметров.
\param context Контекст проверяемой информации. Для отображение в выводимой информации внутри чего совершается ошибка.
\param resultsCount Какое количество результатов должно присутствовать в ответе. Не меньше. Возможно больше.*/
int CheckParameters(Messenger* messenger, db::Response *response, char *context, int resultsCount);

/** Один из результатов запроса в базу данных. Так как в одном запросе может быть несколько команд, то один ответ от базы данных может содержать несколько результатов.
Все строки и столбцы исчисляются начиная с нуля.*/
class Result
{
public:
	
	/** Возвращает true если указанное поле не было определено.*/
	virtual bool IsNull(const int rowIndex, const int columnIndex) = 0;
	virtual bool IsNull(const int rowIndex, const char* columnIndex) = 0;
	
	/** Прочитать булевое значение из результата. В случае какой-либо ошибки возвращает false.*/
	virtual bool GetBool(const int rowIndex, const int columnIndex) = 0;
	virtual bool GetBool(const int rowIndex, const char* column) = 0;

	/** Прочитать целочисленное значение из результата. В случае какой-либо ошибки возвращает -3.*/
	virtual int32_t GetInt32(const int rowIndex, const int columnIndex) = 0;
	virtual int32_t GetInt32(const int rowIndex, const char *column) = 0;
	virtual int64_t GetInt64(const int rowIndex, const int columnIndex) = 0;
	virtual int64_t GetInt64(const int rowIndex, const char *column) = 0;

	/** Прочитать вещественное значение из результата. В случае какой-либо ошибки возвращает -3.*/
	virtual float GetFloat(const int rowIndex, const int columnIndex) = 0;
	virtual float GetFloat(const int rowIndex, const char *column) = 0;
	virtual double GetDouble(const int rowIndex, const int columnIndex) = 0;
	virtual double GetDouble(const int rowIndex, const char *column) = 0;
	
	/** Прочитать строку из результата. В случае какой-либо ошибки возвращает "error".*/
	virtual const char* GetString(const int rowIndex, const int columnIndex) = 0;
	virtual const char* GetString(const int rowIndex, const char *column) = 0;

	/** Количество строк в ответе.*/
	virtual size_t GetRowsCount() = 0;

	/** Количество столбцов в каждой строке.*/
	virtual size_t GetColumnsCount() = 0;

	/** Печать всех строк результата.*/
	virtual void Print(Messenger *messenger) = 0;
};

/** Ответ от базы данных. Может содержать сразу несколько результатов, так как в одном запросе в базу данных может содержаться сразу несколько команд.*/
class Response
{
public:
	
	/** Виртуальный деструктор для возможности использования "умных" указателей на результат из базы данных.*/
	virtual ~Response();
	
	/** Количество результатов (зависит от того сколько команд было отправлено в базу данных в запросе.*/
	virtual int GetResultsCount() = 0;

	/** Получить результат под указанным индексом. Отсчёт от нуля.
	В случае каких-либо ошибок при получении результата будет возвращён пустой результат по-умолчанию - НЕ нулевой указатель.*/
	virtual Result* GetResult(int index) = 0;

	/** Печать всех результатов.*/
	virtual void Print(Messenger *messenger) = 0;

	/** true если запрос в базу данных прошёл успешно (не было синтаксических ошибок и так далее).*/
	virtual bool IsOk() = 0;
};

/** "Умный" указатель на результат запроса в базу данных.
thread-unsafe.*/
class ResponseRef
{
public:
	
	/** Без указателя ничего удаляться и не будет.*/
	ResponseRef();
	
	/** Может указывать на NULL в случае корректной обработки какой-либо ошибки.*/
	ResponseRef( Response* response );
	
	/** Удаление результата если больше не осталось указателей на него.*/
	~ResponseRef();
	
	/** Копирование результата.*/
	ResponseRef( const ResponseRef& responseRef );
	
	ResponseRef& operator=( const ResponseRef& responseRef );
	
	/** Количество результатов (зависит от того сколько команд было отправлено в базу данных в запросе.*/
	int GetResultsCount();
	
	/** Получить результат под указанным индексом. Отсчёт от нуля.
	В случае каких-либо ошибок при получении результата будет возвращён пустой результат по-умолчанию - НЕ нулевой указатель.*/
	Result* GetResult( int index );
	
	/** Печать всех результатов.*/
	void Print( Messenger *messenger );

	/** true если запрос в базу данных прошёл успешно (не было синтаксических ошибок и так далее).*/
	bool IsOk();
	
private:
	
	void Copy( const ResponseRef& responseRef );
	
	void Delete();
	
	/** Чтобы нельзя было взять адрес.*/
	int operator&();
	
	/** Адрес переменной, в которой хранится количество указателей на сам результат.*/
	int* references;
	
	/** Сам результат из базы данных.*/
	Response* response;
	
	/** Используется при удалении "ссылки".*/
	static boost::mutex deletionMutex;
};

/** Тип callback функции, вызываемой при получении от базы данных результата запроса.
\param responsePtr Результат запроса в базу данных.
\param data Произвольные данные, которые были указаны при отправке запроса в базу данных.*/
typedef void(*Callback)(const ResponseRef& responseRef, void *data);

/** Интерфейс к базе данных.
Результат, полученный от базы данных, не должен где-либо сохраняться - он будет (в PostgreSQL - да) удалён после вызова функции обратного вызова.*/
class Interface
{
public:

	/** Подключение. Конструктор реализации не должен как-нибудь обременять - какие-либо первоначальные действия должны происходить не раньше чем в этой функции.
	Если соединение устанавливается нормально, то возвращается ноль.*/
	virtual int Connect( const char* dbName, const char* login, const char* password ) = 0;

	/** Отправить запрос в базу данных.
	Если какой-либо запрос уже отправлен в базу данных (то есть ожидается результат этого запроса от базы данных), то текущий запрос будет добавлен в очередь.
	Отправлять запросы к БД (вызывать эту функцию) можно только после Connect().
	\param queryString Строка запроса. Память освобождается внутри по результатам получения ответа от базы данных.
	\param callback Функция обратного вызова, которая будет вызвана с передачей параметра при получении результата запроса от базы данных.
	\param parameter Произвольные данные, которые будут переданы в callback при её вызове.*/
	virtual void SendQuery( char* queryString, Callback callback = NULL, void* parameter = NULL ) = 0;

	/** Необходимо (в зависимости от реализации) вызывать при выходе из программы (предположительно для прекращения соединения).
	Функция не обязательно будет использоваться реализацией, поэтому здесь это не чистая виртуальная функция, а пустышка.*/
	virtual void Exit();
	
	/** Возвращает true если указанное сообщение проходит через фильтр заблокированных посредством AddFilter() сообщений об ошибках из базы данных.*/
	bool PassFilter( const char* msg );
	/** Добавить фильтр вывода сообщений об ошибках из базы данных. Если сообщение об ошибке содержит указанную подстроку, то сообщение выводиться не будет.*/
	void AddFilter( const char* msg );
	/** Удалить прежде добавленный посредством AddFilter() фильтр.*/
	void RemoveFilter( const char* msg );

private:
	/** Все имеющиеся на данный момент фильтры.*/
	std::vector< std::string > filters;
	/** При использовании filters.*/
	boost::mutex filtersMutex;
};

}//namespace db

#endif//#ifndef DB_INTERFACE_H




