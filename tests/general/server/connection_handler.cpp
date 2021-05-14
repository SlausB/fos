

#include "handlers.h"

using namespace fos;

void ConnectionHandler(const uint64_t sessionId, Batch* luggage, const std::string& ipAddress, ClientTime* clientTime)
{
	Globals::messenger->write(boost::format("I: ConnectionHandler(): %lld connected.\n") % sessionId);
}

