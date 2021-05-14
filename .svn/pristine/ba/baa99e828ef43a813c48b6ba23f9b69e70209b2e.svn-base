

#include "client.h"

#include "externs.h"

#include "memdbg_start.h"

using namespace fos;

Client::Client(const char type, const uint64_t& sessionId, bool isOnHandle, Batch* luggage, const std::string& ipAddress): type(type), sessionId(sessionId), isOnHandle(isOnHandle), luggage(luggage), ipAddress(ipAddress)
{
}

Client::Event::Event(const char type): type(type)
{
}

Client::Event::~Event()
{
	switch(type)
	{
	case Client::Event::CONNECTION:
		delete ipAddress;
		break;
	case Client::Event::MESSAGE:
		free(messageData);
		break;
	case Client::Event::DISCONNECTION:
		break;
	case Client::Event::DROP_BATCH:
		delete batchId;
		break;
	default:
		Globals::messenger->write(boost::format("E: Client::Event::~Event(): type = %d is undefined.\n") % type);
		break;
	}
}

#include "memdbg_end.h"

