
#include <iostream>

#include "stream/stream.h"

#include "../messages.h"

#include "../../../src/output/messenger.h"
#include "../../../src/output/outputs/console_output.h"

#include "../../../src/sync/context_lock.h"

#include "../game.h"

#include "comm.h"

#include "timer.h"

Messenger messenger(new ConsoleOutput);

int errorsCount;

#define TEST_ERROR(message) messenger.error(message); errorsCount++; goto END;
#define TEST_ERROR_F(message) messenger.error(message); errorsCount++; return true;
#define TEST_OK(message) messenger.write(message);
#define MSG(message) messenger.write(message);
#define CALL(function) if(function) goto END;

bool GetMemUsage(Communication* comm, int32_t& memUsage, const char* context)
{
	OutStream outStream;
	outStream << ClientToServer::GET_MEM_USAGE;
	comm->Write(outStream);

	InStream inStream(comm->Read());
	int32_t inMsgType;
	inStream >> inMsgType;
	if(inMsgType != ServerToClient::MEM_USAGE)
	{
		TEST_ERROR_F(boost::format("E: %s: got %d message instead of %d.\n") % context % inMsgType % (int)ServerToClient::MEM_USAGE);
	}
	else
	{
		inStream >> memUsage;
		if(inStream.LeftBytes() != 0)
		{
			TEST_ERROR_F(boost::format("E: %s left %d bytes instead of 0.\n") % context % inStream.LeftBytes());
		}
	}
	return false;
}

void Dump()
{
	Communication comm("don't matter", &messenger);
	OutStream outStream;
	outStream << ClientToServer::DUMP_MEMORY_LEAKS;
	comm.Write(outStream);
}

int main(int argc, char* argv[])
{
	setlocale(LC_CTYPE, "");

	messenger.write("client:\n");

	enum
	{
		FOX,
		BEAR,
		WOLF,
		FUNGUS,
		CROW,
		CLIENTS_COUNT,
	};

	std::vector<std::string> clientsNames;
	clientsNames.push_back("fox");
	clientsNames.push_back("bear");
	clientsNames.push_back("wolf");
	clientsNames.push_back("fungus");
	clientsNames.push_back("crow");

	std::map<int, Communication*> clients;



	//поиск причины [возможных] утечек памяти:
	{
		/*//просто подключение-отключение и dump:
		{
			Dump();
			goto END;
		}*/

		/*//отправка большого количества больших сообщений:
		{
			Communication comm("don't matter", &messenger);

			//получение количества используемой памяти перед тестом:
			{
				int32_t memUsage;
				CALL(GetMemUsage(&comm, memUsage, "mem test: get mem 1"));
				MSG(boost::format("I: mem test: msg: bytes before: %d.\n") % memUsage);
			}

			std::vector<int32_t> data;
			data.resize(8000 / sizeof(int32_t));

			for(int i = 0; i < 1000; i++)
			{
				OutStream outStream;
				outStream << ClientToServer::MEM_LEAK_DETECT << data;
				comm.Write(outStream);

				InStream inStream(comm.Read());
				int inMsgType;
				inStream >> inMsgType;
				if(inMsgType != ServerToClient::MEM_LEAK_DETECT_RESULT)
				{
					TEST_ERROR(boost::format("E: mem test: msg: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::MEM_LEAK_DETECT_RESULT);
					break;
				}
				else
				{
					int result;
					inStream >> result;
					if(result != 1)
					{
						TEST_ERROR(boost::format("E: mem test: msg: result = %d instead of 1.\n") % result);
						break;
					}
				}
			}

			//получение количества используемой памяти после теста:
			{
				int32_t memUsage;
				CALL(GetMemUsage(&comm, memUsage, "mem test: get mem 2"));
				MSG(boost::format("I: mem test: msg: bytes  after: %d.\n") % memUsage);
			}
		}

		//много подключений и отключений:
		{
			//для получения количества памяти:
			Communication comm("don't matter", &messenger);

			//получение количества используемой памяти перед тестом:
			{
				int32_t memUsage;
				CALL(GetMemUsage(&comm, memUsage, "mem test: conn: get mem 1"));
				MSG(boost::format("I: mem test: conn: msg: bytes before: %d.\n") % memUsage);
			}

			for(int i = 0; i < 1000; i++)
			{
				Communication communication("don't matter", &messenger);
			}

			//получение количества используемой памяти после теста:
			{
				int32_t memUsage;
				CALL(GetMemUsage(&comm, memUsage, "mem test: conn: get mem 2"));
				MSG(boost::format("I: mem test: conn: msg: bytes  after: %d.\n") % memUsage);
			}
		}

		//подключения вместе с сообщениями:
		{
			//для получения количества памяти:
			Communication comm("don't matter", &messenger);

			//получение количества используемой памяти перед тестом:
			{
				int32_t memUsage;
				CALL(GetMemUsage(&comm, memUsage, "mem test: mixed: get mem 1"));
				MSG(boost::format("I: mem test: mixed: msg: bytes before: %d.\n") % memUsage);
			}

			for(int i = 0; i < 5; i++)
			{
				Communication communication("don't matter", &messenger);

				OutStream outStream;
				outStream << ClientToServer::PING_ME;
				communication.Write(outStream);
				
				InStream inStream(communication.Read());
				int32_t inMsgType;
				inStream >> inMsgType;
				if(inMsgType != ServerToClient::PING)
				{
					TEST_ERROR(boost::format("E: mem test: mixed: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::PING);
				}
				else
				{
					if(inStream.LeftBytes() != 0)
					{
						TEST_ERROR(boost::format("E: mem test: mixed: left %d bytes intead of 0.\n") % inStream.LeftBytes());
					}
				}
			}

			//получение количества используемой памяти после теста:
			{
				int32_t memUsage;
				CALL(GetMemUsage(&comm, memUsage, "mem test: mixed: get mem 2"));
				MSG(boost::format("I: mem test: mixed: msg: bytes  after: %d.\n") % memUsage);
			}
		}

		//использование большого количества "тяжёлых" данных:
		{
			{
				Communication comm("don't matter", &messenger);

				for(int i = 0; i < 10; i++)
				{
					OutStream outStream;
					outStream << ClientToServer::USE_BATCH << i;
					comm.Write(outStream);
				}
			}

			Dump();
		}*/

		////для поиска утечек остальной спам мешать не должен:
		//goto END;
	}

	/*//тест "вообще работает ли":
	for(int i = 0; i < 1; i++)
	{
		boost::thread clientThread(RunClient, i);
		boost::this_thread::sleep(boost::posix_time::milliseconds(3));
	}

	boost::this_thread::sleep(boost::posix_time::milliseconds(500));*/

	//для теста на конкретное взаимодействие клиентов:
	for(int i = 0; i < CLIENTS_COUNT; i++)
	{
		clients[i] = new Communication(clientsNames[i], &messenger);
	}

	//подсчёт пинга:
	{
		const int count = 500;
		OutStream outStream;
		outStream << ClientToServer::PING_ME;
		bool isTested = true;
		Timer timer;
		for(int i = 0; i < count; i++)
		{
			clients[FOX]->Write(outStream);
			InStream inStream(clients[FOX]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::PING)
			{
				TEST_ERROR(boost::format("E: ping: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::PING);
				isTested = false;
				break;
			}
		}
		if(isTested)
		{
			TEST_OK(boost::format("I: ping: average ping is %.6f seconds.\n") % (timer.getElapsedTime() / (double)count));
		}
		else
		{
			TEST_ERROR(boost::format("E: ping: test failed because of some reasons.\n"));
		}
	}

	//отправка и чтение очень большого сообщения:
	{
		OutStream outStream;
		outStream << ClientToServer::GET_VERY_BIG_MESSAGE;
		for(int i = 0; i < 2000; i++)
		{
			outStream << i;
		}
		messenger << boost::format("I: sending %d bytes.\n") % outStream.GetData().size();
		clients[FOX]->Write(outStream);

		InStream inStream(clients[FOX]->Read());
		int32_t inMsgType;
		inStream >> inMsgType;
		if(inMsgType != ServerToClient::VERY_BIG_MESSAGE)
		{
			TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::VERY_BIG_MESSAGE);
		}
		else
		{
			TEST_OK(boost::format("I: OK: ServerToClient::VERY_BIG_MESSAGE got.\n"));
		}
		const int leftBytes = inStream.LeftBytes();
		if(leftBytes != 2000 * sizeof(int32_t))
		{
			TEST_ERROR(boost::format("E: left %d bytes instead of 2000 * %d.\n") % leftBytes % sizeof(int32_t));
		}
		else
		{
			TEST_OK(boost::format("I: OK: left 2000 * %d bytes.\n") % sizeof(int32_t));
		}
		bool allOk = true;
		for(int i = 0; i < 2000; i++)
		{
			int32_t value;
			inStream >> value;
			if(value != i)
			{
				TEST_ERROR(boost::format("E: value must equal %d but it's %d.\n") % i % value);
				allOk = false;
			}
		}
		if(allOk)
		{
			TEST_OK(boost::format("I: OK: all 2000 integers successfully read.\n"));
		}
	}

	//отправка сообщения с задержкой и дисконнект:
	{
		OutStream outStream;
		outStream << ClientToServer::GET_VERY_LONG_MESSAGE;
		clients[FOX]->Write(outStream);
		
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

		delete clients[FOX];
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
		clients[FOX] = new Communication(clientsNames[FOX], &messenger);
	}

	//чтение сообщения с задержкой:
	{
		OutStream outStream;
		outStream << ClientToServer::GET_VERY_LONG_MESSAGE;
		clients[FOX]->Write(outStream);

		InStream inStream(clients[FOX]->Read());
		int32_t inMsgType;
		inStream >> inMsgType;
		if(inMsgType != ServerToClient::VERY_LONG_MESSAGE)
		{
			TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::VERY_LONG_MESSAGE);
		}
		else
		{
			TEST_OK(boost::format("I: OK: ServerToClient::VERY_LONG_MESSAGE got.\n"));
		}
		const int leftBytes = inStream.LeftBytes();
		if(leftBytes != 10 * sizeof(int32_t))
		{
			TEST_ERROR(boost::format("E: left %d bytes instead of 10 * %d.\n") % leftBytes % sizeof(int32_t));
		}
		else
		{
			TEST_OK(boost::format("I: OK: 10 * %d bytes got.\n") % sizeof(int32_t));
		}
		bool allOk = true;
		for(int i = 0; i < 10; i++)
		{
			int32_t value;
			inStream >> value;
			if(value != i)
			{
				TEST_ERROR(boost::format("E: value must equal %d but it's %d.\n") % i % value);
				allOk = false;
			}
		}
		if(allOk)
		{
			TEST_OK(boost::format("I: OK: all 10 integers successfully read.\n"));
		}
	}

	//логин лисы:
	{
		OutStream outStream;
		outStream << ClientToServer::LOGIN << clientsNames[FOX];
		clients[FOX]->Write(outStream);
	}

	//проверка удаления данных при инициативном отключении (и вообще возможность такого отключения):
	{
		//изначально зданий быть не должно:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[FOX]->Write(outStream);

			MSG(boost::format("I: fox: waiting for buildings to check if it's empty...\n"));

			InStream inStream(clients[FOX]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			std::vector<int32_t> buildings;
			inStream >> buildings;
			const int count = buildings.size();
			if(count != 0)
			{
				TEST_ERROR(boost::format("E: fox: got %d buildings instead of 0.\n") % count);
			}
			else
			{
				TEST_OK(boost::format("I: OK: fox: got 0 buildings.\n"));
			}
		}

		//добавляем одно:
		{
			OutStream outStream;
			outStream << ClientToServer::PUT_BUILDING << Buildings::SCHOOL;
			clients[FOX]->Write(outStream);

			OutStream outStreamB;
			outStreamB << ClientToServer::GET_BUILDINGS;
			clients[FOX]->Write(outStreamB);

			MSG(boost::format("I: fox: waiting for buildings...\n"));
			InStream inStream(clients[FOX]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: fox: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				TEST_OK(boost::format("I: OK: fox: message %d got.\n") % inMsgType);

				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 1)
				{
					TEST_ERROR(boost::format("E: fox: got %d buildings instead of 1.\n") % count);
				}
				else
				{
					TEST_OK(boost::format("I: fox: got 1 building.\n"));

					const int buildingType = buildings[0];
					if(buildingType != Buildings::SCHOOL)
					{
						TEST_ERROR(boost::format("E: fox: got %d building type instead of %d.\n") % buildingType % (int)Buildings::SCHOOL);
					}
					else
					{
						TEST_OK(boost::format("I: OK: fox: building type %d got.\n") % buildingType);
					}
				}
			}
		}

		//удаляем лису со стороны сервера:
		{
			OutStream outStream;
			outStream << ClientToServer::DISCONNECT_ME;
			clients[FOX]->Write(outStream);

			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

			delete clients[FOX];
			clients[FOX] = new Communication(clientsNames[FOX], &messenger);
		}

		//медведь проверяет что домики удалились:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: bear: waiting for buildings...\n"));

			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				TEST_OK(boost::format("I: OK: message %d got.\n") % inMsgType);

				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 0)
				{
					TEST_ERROR(boost::format("E: bear: got %d buildings instead of 0.\n") % count);
				}
				else
				{
					TEST_OK(boost::format("I: OK: bear: got 0 buildings - they was successfully removed through initiated disconnection.\n"));
				}
			}
		}

		//переподключаем медведя чтобы он перестал следить за данными:
		{
			delete clients[BEAR];
			clients[BEAR] = new Communication(clientsNames[BEAR], &messenger);
		}
	}

	//проверка удаления данных:
	{
		//изначально зданий быть не должно:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[FOX]->Write(outStream.GetData());

			InStream inStream(clients[FOX]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			std::vector<int32_t> buildings;
			inStream >> buildings;
			const int count = buildings.size();
			if(count != 0)
			{
				TEST_ERROR(boost::format("E: there are %d buildings instead of 0.\n") % count);
			}
			else
			{
				TEST_OK(boost::format("I: OK: 0 buildings.\n"));
			}
		}

		//добавляем одно здание:
		{
			OutStream outStream;
			outStream << ClientToServer::PUT_BUILDING << Buildings::POLICE;
			clients[FOX]->Write(outStream);

			OutStream outStreamB;
			outStreamB << ClientToServer::GET_BUILDINGS;
			clients[FOX]->Write(outStreamB);

			InStream inStream(clients[FOX]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				TEST_OK(boost::format("I: OK: message %d got.\n") % inMsgType);
			}
			std::vector<int32_t> buildings;
			inStream >> buildings;
			const int buildingsCount = buildings.size();
			if(buildingsCount != 1)
			{
				TEST_ERROR(boost::format("E: got %d buildings instead of 1.\n") % buildingsCount);
			}
			else
			{
				TEST_OK(boost::format("I: OK: got 1 building.\n"));
				const int type = buildings[0];
				if(type != 1)
				{
					TEST_ERROR(boost::format("E: got building type = %d instead of %d.\n") % type % (int)Buildings::POLICE);
				}
				else
				{
					TEST_OK(boost::format("I: OK: got building type = %d.\n") % (int)Buildings::POLICE);
				}
			}
		}

		//удаляемся - данные должны будут удалиться:
		delete clients[FOX];
		clients[FOX] = new Communication(clientsNames[FOX], &messenger);
		//проверка что зданий теперь снова ноль:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[FOX]->Write(outStream);

			InStream inStream(clients[FOX]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				TEST_OK(boost::format("I: OK: message %d got.\n") % inMsgType);
			}
			std::vector<int32_t> buildings;
			inStream >> buildings;
			const int buildingsCount = buildings.size();
			if(buildingsCount != 0)
			{
				TEST_ERROR(boost::format("E: got %d buildings. They was NOT removed.\n") % buildingsCount);
			}
			else
			{
				TEST_OK(boost::format("I: OK: got %d buildings. They was successfully removed.\n") % buildingsCount);
			}
		}
	}

	//использование данных с позиций двух; отключение одного - данные должны существовать; отключение второго - данные должны уничтожиться:
	{
		//использование данных лисой:
		{
			OutStream outStream;
			outStream << ClientToServer::PUT_BUILDING << Buildings::SCHOOL;
			clients[FOX]->Write(outStream);
		}

		//использование данных медведем:
		{
			OutStream outStream;
			outStream << ClientToServer::PUT_BUILDING << Buildings::SHOP;
			clients[BEAR]->Write(outStream);
		}

		//проверка получившихся данных:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: bear: waiting for buildings 1...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int buildingsCount = buildings.size();
				if(buildingsCount != 2)
				{
					TEST_ERROR(boost::format("E: got %d buildings instead of 2.\n") % buildingsCount);
				}
				else
				{
					const int firstBuildingType = buildings[0];
					if(firstBuildingType != Buildings::SCHOOL)
					{
						TEST_ERROR(boost::format("E: got %d first building instead of %d.\n") % firstBuildingType);
					}
					else
					{
						const int secondBuildingType = buildings[1];
						if(secondBuildingType != Buildings::SHOP)
						{
							TEST_ERROR(boost::format("E: got %d second building instead of %d.\n") % secondBuildingType);
						}
					}
				}
			}
		}

		//отключение лисы:
		{
			delete clients[FOX];
			clients[FOX] = new Communication(clientsNames[FOX], &messenger);
		}

		//после дисконнекта лисы данные всё ещё должны остаться:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: bear: waiting for buildings 2...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int buildingsCount = buildings.size();
				if(buildingsCount != 2)
				{
					TEST_ERROR(boost::format("E: got %d buildings instead of 2.\n") % buildingsCount);
				}
				else
				{
					const int firstBuildingType = buildings[0];
					if(firstBuildingType != Buildings::SCHOOL)
					{
						TEST_ERROR(boost::format("E: got %d first building instead of %d.\n") % firstBuildingType);
					}
					const int secondBuildingType = buildings[1];
					if(secondBuildingType != Buildings::SHOP)
					{
						TEST_ERROR(boost::format("E: got %d second building instead of %d.\n") % secondBuildingType);
					}
				}
			}
		}

		//переподключение медведя:
		{
			MSG(boost::format("I: bear reconnection...\n"));
			delete clients[BEAR];
			clients[BEAR] = new Communication(clientsNames[BEAR], &messenger);
		}

		//а вот после отключения медведя данные уже должны исчезнуть:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: bear: waiting for buildings 3...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int buildingsCount = buildings.size();
				if(buildingsCount != 0)
				{
					TEST_ERROR(boost::format("E: got %d buildings instead of 0.\n") % buildingsCount);
				}
			}
		}

		//переподключаем медведя для прекращения использования данных:
		{
			delete clients[BEAR];
			clients[BEAR] = new Communication(clientsNames[BEAR], &messenger);
		}
	}

	//прекращение использования данных по инициативе лисы и медведя:
	{
		//лиса использует данные:
		{
			OutStream outStream;
			outStream << ClientToServer::PUT_BUILDING << Buildings::POLICE;
			clients[FOX]->Write(outStream);
		}

		//медведь тоже использует здания и заодно проверяет их количество:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: drop: bear: 1a: waiting for buildings...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: drop: bear: 1a: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 1)
				{
					TEST_ERROR(boost::format("E: drop: bear: 1a: got %d buildings instead of 1.\n") % count);
				}
				else
				{
					const int type = buildings[0];
					if(type != Buildings::POLICE)
					{
						TEST_ERROR(boost::format("E: drop: bear: 1a: got %d building instead of %d.\n") % type % (int)Buildings::POLICE);
					}
				}
			}
		}

		//лиса бросает данные по собственной инициативе:
		{
			OutStream outStream;
			outStream << ClientToServer::DROP_BUILDINGS;
			clients[FOX]->Write(outStream);
		}

		//ещё раз проверяем здания медведем:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: drop: bear: 2a: waiting for buildings...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: drop: bear: 2a: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 1)
				{
					TEST_ERROR(boost::format("E: drop: bear: 2a: got %d buildings instead of 1.\n") % count);
				}
				else
				{
					const int type = buildings[0];
					if(type != Buildings::POLICE)
					{
						TEST_ERROR(boost::format("E: drop: bear: 2a: got %d building instead of %d.\n") % type % (int)Buildings::POLICE);
					}
				}
			}
		}

		//медведь тоже бросает данные по собственной инициативе:
		{
			OutStream outStream;
			outStream << ClientToServer::DROP_BUILDINGS;
			clients[BEAR]->Write(outStream);
		}

		//теперь зданий быть не должно:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: drop: bear: 3a: waiting for buildings...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: drop: bear: 3a: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 0)
				{
					TEST_ERROR(boost::format("E: drop: bear: 3a: got %d buildings instead of 0.\n") % count);
				}
			}
		}
	}

	//прекращение использования данных по инициативе лисы, но по отключению медведя:
	{
		//лиса использует данные:
		{
			OutStream outStream;
			outStream << ClientToServer::PUT_BUILDING << Buildings::POLICE;
			clients[FOX]->Write(outStream);
		}

		//медведь тоже использует здания и заодно проверяет их количество:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: drop: bear: 1b: waiting for buildings...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: drop: bear: 1b: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 1)
				{
					TEST_ERROR(boost::format("E: drop: bear: 1b: got %d buildings instead of 1.\n") % count);
				}
				else
				{
					const int type = buildings[0];
					if(type != Buildings::POLICE)
					{
						TEST_ERROR(boost::format("E: drop: bear: 1b: got %d building instead of %d.\n") % type % (int)Buildings::POLICE);
					}
				}
			}
		}

		//лиса бросает данные по собственной инициативе:
		{
			OutStream outStream;
			outStream << ClientToServer::DROP_BUILDINGS;
			clients[FOX]->Write(outStream);
		}

		//ещё раз проверяем здания медведем:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: drop: bear: 2b: waiting for buildings...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: drop: bear: 2b: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 1)
				{
					TEST_ERROR(boost::format("E: drop: bear: 2b: got %d buildings instead of 1.\n") % count);
				}
				else
				{
					const int type = buildings[0];
					if(type != Buildings::POLICE)
					{
						TEST_ERROR(boost::format("E: drop: bear: 2b: got %d building instead of %d.\n") % type % (int)Buildings::POLICE);
					}
				}
			}
		}

		//медведь тоже бросает данные через переподключение:
		{
			delete clients[BEAR];
			clients[BEAR] = new Communication(clientsNames[BEAR], &messenger);
		}

		//теперь зданий быть не должно:
		{
			OutStream outStream;
			outStream << ClientToServer::GET_BUILDINGS;
			clients[BEAR]->Write(outStream);

			MSG(boost::format("I: drop: bear: 3b: waiting for buildings...\n"));
			InStream inStream(clients[BEAR]->Read());
			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::BUILDINGS)
			{
				TEST_ERROR(boost::format("E: drop: bear: 3b: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				const int count = buildings.size();
				if(count != 0)
				{
					TEST_ERROR(boost::format("E: drop: bear: 3b: got %d buildings instead of 0.\n") % count);
				}
			}
		}
	}

	//ожадание данных, освобождённых посредством DropBatch() (тест неверный - данные так-и-так освобождаются от использования после обработки сообщения - для корректной реализации теста необходимо устраивать перекрёстный запрос данных (создавать deadlock) и, затем, в одном из потоков (на сервере) освобождать данные посредством DropBatch() - пока лень это делать):
	{
		//переподключение лисы и медведя чтобы сбросить использование данных:
		{
			delete clients[FOX];
			clients[FOX] = new Communication(clientsNames[FOX], &messenger);

			delete clients[BEAR];
			clients[BEAR] = new Communication(clientsNames[BEAR], &messenger);
		}

		//использование данных лисой:
		{
			OutStream outStream;
			outStream << ClientToServer::LONG_PUT_BUILDING << Buildings::POLICE;
			clients[FOX]->Write(outStream);
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(500));

		//использование тех же данных медведем:
		{
			OutStream outStream;
			outStream << ClientToServer::LONG_GET_BUILDINGS;
			clients[BEAR]->Write(outStream);
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));

		//лиса освобождает данные:
		{
			OutStream outStream;
			outStream << ClientToServer::LONG_DROP_BUILDINGS;
			clients[FOX]->Write(outStream);
		}

		//теперь медведь должен быть способен получить данные:
		{
			MSG(boost::format("I: long drop: waiting for buildings...\n"));
			InStream inStream(clients[BEAR]->Read());

			int32_t inMsgType;
			inStream >> inMsgType;
			if(inMsgType != ServerToClient::LONG_BUILDINGS)
			{
				TEST_ERROR(boost::format("E: long drop: got %d message instead of %d.\n") % inMsgType % (int)ServerToClient::LONG_BUILDINGS);
			}
			else
			{
				std::vector<int32_t> buildings;
				inStream >> buildings;
				if(inStream.LeftBytes() != 0)
				{
					TEST_ERROR(boost::format("E: long drop: left %d bytes instead of 0.\n") % inStream.LeftBytes());
				}
				else
				{
					const int count = buildings.size();
					if(count != 1)
					{
						TEST_ERROR(boost::format("E: long drop: got %d buildings instead of 1.\n") % count);
					}
					else
					{
						const int type = buildings[0];
						if(type != Buildings::POLICE)
						{
							TEST_ERROR(boost::format("E: long drop: got %d building type instead of %d.\n") % type % (int)Buildings::POLICE);
						}
					}
				}
			}
		}
	}

	//лиса строит десять зданий:
	{
		for(int i = 0; i < 10; i++)
		{
			OutStream outStream;
			//outStream << ClientToServer::
		}
	}

	//последовательная обработка входящих сообщений:
	{
		for(int i = 0; i < 10000; i++)
		{

		}
	}

	//тест на устойчивость:
	{
		MSG(boost::format("I: ===============================\nI: Testing much connections acception...\n"));
		const int CONNECTIONS_COUNT = 1000;
		MSG(boost::format("I: trying to raise %d connections.\n") % CONNECTIONS_COUNT);
		for(int i = 0; i < CONNECTIONS_COUNT; i++)
		{
			Communication* comm = new Communication("don't matter", &messenger);
		}
	}

END:

	if(errorsCount > 0)
	{
		messenger << boost::format("E: all done: there was %d errors.\n") % errorsCount;
	}
	else
	{
		messenger << boost::format("I: all done without errors.\n");
	}

	getchar();

	return 0;
}



