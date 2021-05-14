
/** Последовательный доступ к базе данных.*/

#ifndef PQL_H
#define PQL_H

#include "../output/messenger.h"

#include <deque>

//PostgreSQL:
#include "libpq-fe.h"

#include "db_iface.h"

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

namespace pq_db
{

/** Соединение с PostgreSQL database.
Использование:
\code

Messenger *messenger	= new Messenger("someFile.txt");
PqlDbConn *pqlDbConn	= new PqlDbConn(messenger);
if(pqlDbConn->Connect() != 0)
{
	//обработка ошибка...
}

//функция, которая будет вызвана при получении ответа от базы данных:
void resultHandler(std::vector<PGresult*> *wholeResult, void* parameter)
{
	//поработали и очищаем "wholeResult" самостоятельно:
	for(int i = 0; i < wholeResult->size(); i++)
	{
		PQclear(wholeResult->at(i));
	}
	delete wholeResult;
}

pqlDbConn->SendQuery(

...

//выход из приложения:
pqlDbConn->Exit();

\endcode
*/
class PqlDbConn: public ::db::Interface
{
	friend void Communication(PqlDbConn* pqlDbConn);
	
public:
	/** Создать экземпляр этого класса можно где и когда угодно. Для фактического соединения нужно вызвать Connect().*/
	PqlDbConn(Messenger *messenger);
	
	/** Деструктор. Корректно удаляет соединение. Все вызовы SendQuery() после (в процессе) вызова деструктора ведут к неопределённому поведению.*/
	~PqlDbConn();
	
	/** Соединение с базой данных. Отправлять запросы в БД можно только после вызова этой функции.
	Если всё нормально, то возвращает 0.*/
	int Connect(const char* dbName, const char* login, const char* password);
	
	/** Отправка запроса в базу данных.
	\param queryString Будет освобождено внутри по результатам получения ответа за запрос.*/
	void SendQuery( char* queryString, ::db::Callback callback = NULL, void* parameter = NULL );
	
	/** Вызывать при выходе из приложения.*/
	void Exit();
	
private:
	
	/** Запрос в базу данных, добавленный в очередь.*/
	class Query
	{
	public:

		/** Обязательный конструктор.
		\param queryString Память будет освобождена в деструкторе.
		*/
		Query( char* queryString, db::Callback callback, void* parameter );
		
		/** Освобождение queryString.*/
		~Query();

		/** Сам текст запроса.*/
		char* queryString;
		
		/** callback-функция, которая будет вызвана после получения результатов запроса от базы данных.*/
		db::Callback callback;
		
		/** Параметр, с которым будет вызвана callback-функция.*/
		void *parameter;
	};


	/** Куда пишутся всякие сообщения.*/
	Messenger *messenger;

	/** Созданное в Connect() соединение.*/
	PGconn *conn;

	/** Запросы, которые были добавлены пока ожидается ответ от базы данных для последнего запроса.*/
	std::deque< Query* > queries;

	/** Мьютекс чтения/добавления (pop/push) запросов в очередь для последующей отправки в базу данных.*/
	boost::mutex queriesMutex;

	/** Для остановки процесса общения с базой данных.*/
	boost::mutex commLockMutex;
	boost::condition thereAreQueries;
	
	/** true - соединение с базой данный сейчас удаляется.*/
	bool isStopping;
	/** true - функция Communication() когда-либо вызывалась.*/
	bool isCommunicating;
	/** true - функция Communication() завершилась.*/
	bool isCommunicationExited;
	/** Используется для ожидания остановки потока функции Communication().*/
	boost::mutex stoppingMutex;

public:	
	boost::mutex statsMutex;
	uint64_t requestsCount;
	double grossSeconds;
	double longestRequestSeconds;
	std::string longestRequest;
	double fastestRequestSeconds;
	std::string fastestRequest;
};

}//namespace pq_db

#endif//#ifndef PQL_H

