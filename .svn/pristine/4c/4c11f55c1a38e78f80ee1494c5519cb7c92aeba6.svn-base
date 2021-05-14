

#include "handlers.h"

#include "../../../src/stream/stream.h"

#include "../messages.h"

#include "general_batch.h"

#include "mem_usage.h"

using namespace fos;

#define READ(operation) try { operation; } catch(std::exception& e) { Globals::messenger->write(boost::format("E: reading failed with exception \"%s\".\n") % e.what()); return; }

/** Пример обработчика входящих сообщений.*/
void MessageHandler(const uint64_t sessionId, const char* messageData, const size_t messageLength, Batch* luggage, ClientTime* clientTime)
{
	//Globals::messenger->write(boost::format("I: MessageHandler(): got message of length %d from %llu.\n") % messageData.size() % sessionId);

	InStream inStream(messageData, messageLength);

	int32_t inMsgType;
	READ(inStream >> inMsgType);
	switch(inMsgType)
	{
	case ClientToServer::HELLO:
		{
			OutStream outStream;
			int32_t outMsgType = ServerToClient::HELLO;
			outStream << outMsgType;
			Outcoming outcoming(sessionId, outStream.GetData());
			clientTime->Send(outcoming);
			Globals::messenger->write(boost::format("I: MessageHandler(): \"HELLO\" sent.\n"));
		}
		break;

	case ClientToServer::GET_VERY_BIG_MESSAGE:
		{
			const int leftBytes = inStream.LeftBytes();
			if(leftBytes != 2000 * sizeof(int32_t))
			{
				Globals::messenger->write(boost::format("E: left %d bytes instead of 2000 * %d.\n") % leftBytes % sizeof(int32_t));
			}
			for(int i = 0; i < 2000; i++)
			{
				int32_t value;
				inStream >> value;
				if(value != i)
				{
					Globals::messenger->write(boost::format("E: value must equal %d but it's %d.\n") % i % value);
				}
			}

			OutStream outStream;
			outStream << ServerToClient::VERY_BIG_MESSAGE;
			for(int i = 0; i < 2000; i++)
			{
				outStream << i;
			}
			Outcoming outcoming(sessionId, outStream.GetData());
			clientTime->Send(outcoming);
		}
		break;

	case ClientToServer::GET_VERY_LONG_MESSAGE:
		{
			const int leftBytes = inStream.LeftBytes();
			if(leftBytes != 0)
			{
				Globals::messenger->write(boost::format("E: left %d bytes instead of zero.\n") % leftBytes);
			}

			boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

			OutStream outStream;
			outStream << ServerToClient::VERY_LONG_MESSAGE;
			for(int i = 0; i < 10; i++)
			{
				outStream << i;
			}
			Outcoming outcoming(sessionId, outStream.GetData());
			clientTime->Send(outcoming);
		}
		break;

	case ClientToServer::LOGIN:
		{
			std::string name;
			READ(inStream >> name);
			Globals::messenger->write(boost::format("I: MessageHandler(): \"%s\" log in.\n") % name);

			LiteralBatchId literalBatchId(name);
			BatchRef playerBatchRef = clientTime->GetBatch(&literalBatchId);


		}
		break;

	case ClientToServer::GET_BUILDINGS:
		{
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::GET_BUILDINGS: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				BatchRef batchRef = clientTime->GetBatch("buildings");
				BuildingsBatch* buildingsBatch = (BuildingsBatch*)batchRef.batch;
				OutStream outStream;
				outStream << ServerToClient::BUILDINGS << buildingsBatch->types;
				Outcoming outcoming(sessionId, outStream.GetData());
				clientTime->Send(outcoming);
				Globals::messenger->write(boost::format("I: MessageHandler(): %d buildings sent.\n") % buildingsBatch->types.size());
			}
		}
		break;

	case ClientToServer::PUT_BUILDING:
		{
			int32_t buildingType;
			inStream >> buildingType;
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::PUT_BUILDING: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				BatchRef batchRef = clientTime->GetBatch("buildings");
				BuildingsBatch* buildingsBatch = (BuildingsBatch*)batchRef.batch;
				buildingsBatch->types.push_back(buildingType);
			}
		}
		break;

	case ClientToServer::DISCONNECT_ME:
		{
			clientTime->Disconnect(sessionId);
		}
		break;

	case ClientToServer::PING_ME:
		{
			clientTime->Send(OutStream(ServerToClient::PING));
		}
		break;

	case ClientToServer::MEM_LEAK_DETECT:
		{
			std::vector<int32_t> data;
			inStream >> data;
			const int size = data.size();
			OutStream outStream;
			outStream << ServerToClient::MEM_LEAK_DETECT_RESULT;
			if(size != (8000 / sizeof(int32_t)))
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): incoming data has %d integers instead of %d.\n") % size % (8000 / sizeof(int32_t)));
				outStream << 0;
			}
			else
			{
				outStream << 1;
			}
			Outcoming outcoming(sessionId, outStream.GetData());
			clientTime->Send(outcoming);
		}
		break;

	case ClientToServer::GET_MEM_USAGE:
		{
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::GET_MEM_USAGE: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				OutStream outStream;
				outStream << ServerToClient::MEM_USAGE;
				int32_t memUsage = mem_usage();
				outStream << memUsage;
				Outcoming outcoming(sessionId, outStream.GetData());
				clientTime->Send(outcoming);
			}
		}
		break;

	case ClientToServer::DROP_BUILDINGS:
		{
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::DROP_BUILDINGS: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				clientTime->DropBatch("buildings");
			}
		}
		break;

	case ClientToServer::LONG_GET_BUILDINGS:
		{
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::LONG_GET_BUILDINGS: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				BatchRef batchRef = clientTime->GetBatch("buildings");
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
				BuildingsBatch* buildingsBatch = (BuildingsBatch*)batchRef.batch;
				OutStream outStream;
				outStream << ServerToClient::LONG_BUILDINGS << buildingsBatch->types;
				Outcoming outcoming(sessionId, outStream.GetData());
				clientTime->Send(outcoming);
				Globals::messenger->write(boost::format("I: MessageHandler(): %d buildings sent.\n") % buildingsBatch->types.size());
			}
		}
		break;

	case ClientToServer::LONG_PUT_BUILDING:
		{
			int32_t buildingType;
			inStream >> buildingType;
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::LONG_PUT_BUILDING: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				BatchRef batchRef = clientTime->GetBatch("buildings");
				boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
				BuildingsBatch* buildingsBatch = (BuildingsBatch*)batchRef.batch;
				buildingsBatch->types.push_back(buildingType);
			}
		}
		break;

	case ClientToServer::LONG_DROP_BUILDINGS:
		{
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::LONG_DROP_BUILDINGS: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				clientTime->DropBatch("buildings");
			}
		}
		break;

	case ClientToServer::USE_BATCH:
		{
			int32_t id;
			inStream >> id;
			if(inStream.LeftBytes() != 0)
			{
				Globals::messenger->write(boost::format("E: MessageHandler(): ClientToServer::USE_BATCH: left %d bytes instead of 0.\n") % inStream.LeftBytes());
			}
			else
			{
				BatchRef batchRef = clientTime->GetBatch(id);
			}
		}
		break;

	case ClientToServer::DUMP_MEMORY_LEAKS:
		{
#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)
			_CrtDumpMemoryLeaks();
#endif
		}
		break;

	default:
		Globals::messenger->write(boost::format("E: MessageHandler(): incoming message of type = %d is undefined.\n") % inMsgType);
		break;
	}
}

