

#ifndef FOS_RUN_H
#define FOS_RUN_H

#include "meters_factory.h"
#include "batch_factory.h"
#include "handlers.h"
#include <boost/cstdint.hpp>

namespace fos
{

#ifndef FOS_NO_DB

/** Синхронный запуск сервера. Для выхода (остановки сервера) нужно написать в консоль "exit".
startingHandler вызывается внутри этой функции.
\param dbName Имя базы данных, к которой подключаться.
\param dbLogin Логин для подключения к базе данных.
\param dbPassword Пароль для подключения к базе данных.
\param metersFactory Фабрика для создания правил передачи/получения данных. Будет удалена внутри сервера.
\param batchFactory Фабрика для создания данных.
\param startingHandler Вызывается при запуске сервера. Потоко-безопасная: во время выполнения функции в других ничего вызываться не будет.
\param tickHandler Вызывается на протяжении всей жизни сервера с интервалом в tickPeriod секунд.
\param tickPeriod С какой периодичностью секунд вызывать tickHandler .
\param connectionHandler Вызывается при подключении какого-либо клиента.
\param messageHandler Вызывается при получении какого-либо сообщения от клиента.
\param disconnectionHandler Вызывается при отключении клиента.
\param clientsPort Порт, на котором ожидать входящих, от клиентов, соединений.
\param clientData Произвольные данные, которые будут переданы в StartingHandler.*/
void Run(const char* dbName, 
		const char* dbLogin, 
		const char* dbPassword, 
		MetersFactory* metersFactory, 
		BatchFactory* batchFactory, 
		StartingHandler startingHandler, 
		TickHandler tickHandler, 
		const double tickPeriod, 
		ConnectionHandler connectionHandler, 
		MessageHandler messageHandler, 
		DisconnectionHandler disconnectionHandler, 
		ExitingHandler exitingHandler, 
		int clientsPort,
		void* clientData);

#else

/** То же, но без какого-либо соединения с базой данных.*/
void Run(MetersFactory* metersFactory, 
		BatchFactory* batchFactory, 
		StartingHandler startingHandler, 
		TickHandler tickHandler, 
		const double tickPeriod, 
		ConnectionHandler connectionHandler, 
		MessageHandler messageHandler, 
		DisconnectionHandler disconnectionHandler, 
		ExitingHandler exitingHandler, 
		int clientsPort,
		void* clientData);

#endif//#ifndef FOS_NO_DB

}//namespace fos

#endif//#ifndef FOS_RUN_H

