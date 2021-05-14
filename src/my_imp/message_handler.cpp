

#include "handlers.h"

#include "messages.h"

using namespace fos;


#define READ(statement) try { statement; } catch(std::exception& e) { Globals::messenger->write(boost::format("E: MessageHandler(): reading \"%s\" failed with exception \"%s\" at %s:%d for client %llu.\n") % #statement % e.what() % __FILE__ % __LINE__ % sessionId); return; }


void MessageHandler(const uint64_t sessionId, const char* messageData, const size_t messageLength, Batch* luggage, ClientTime* clientTime)
{
	Globals::messenger->write(boost::format("I: MessageHandler(): message of length %u bytes from %llu.\n") % messageLength % sessionId);

	InStream inStream(messageData, messageLength);
	
	uint32_t inMsgType;
	READ(inMsgType = inStream.LEB128_u32());
	
	switch(inMsgType)
	{
		case ClientToServer::PING_ME:
		{
			OutStream outStream;
			outStream.LEB128_u32(ServerToClient::PING);
			clientTime->Send(outStream);
		}
		break;
		
		default:
		Globals::messenger->write(boost::format("W: MessageHandler(): unhandles message of type %u from %llu.\n") % inMsgType % sessionId);
		break;
	}
}

