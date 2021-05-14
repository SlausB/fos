
#include "clients_h.h"

#include <boost/format.hpp>

#include "../../dispatcher.h"

namespace fos
{

Clients_Handler::Clients_Handler(Dispatcher* dispatcher): dispatcher(dispatcher)
{
}

std::vector<std::string> Clients_Handler::GetNames()
{
	std::vector<std::string> names;
	names.push_back("clients");
	return names;
}

void Clients_Handler::GetShortDescription(Messenger* messenger)
{
	messenger->write("prints currently connected clients information.\n");
}

int Clients_Handler::Handle(Colonel::Nodes& nodes, Colonel::Requester* requester)
{
	requester->Listen(str(boost::format("clients info:\n")));
	
	
	dispatcher->clientsMutex.lock();
	dispatcher->batchesMutex.lock();
	
	for(std::map<uint64_t, Client*>::iterator it = dispatcher->clients.begin(); it != dispatcher->clients.end(); it++)
	{
		Client* client = it->second;
		requester->Listen(str(boost::format("	session id: %llu, ip address: \"%s\", batches:\n") % client->sessionId % client->ipAddress));
		requester->Listen(str(boost::format("		luggage: \"%s\"\n") % client->luggage->Describe()));
		for(BatchUsageMap::Iterator it = client->batchesUnderUsage.batches.begin(); it != client->batchesUnderUsage.batches.end(); it++)
		{
			requester->Listen(str(boost::format("		\"%s\"\n") % it->second.usage->batch->Describe()));
		}
	}
	
	dispatcher->batchesMutex.unlock();
	dispatcher->clientsMutex.unlock();
	
	
	return Colonel::KEEP_ISSUING;
}

}//namespace fos


