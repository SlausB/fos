

#include "handlers.h"

using namespace fos;


void DisconnectionHandler(const uint64_t sessionId, Batch* luggage, ClientTime* clientTime)
{
	Globals::messenger->write(boost::format("I: DisconnectionHandler(): %lld disconnected.\n") % sessionId);
}

