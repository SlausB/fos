
#include "conns_h.h"

#include <boost/format.hpp>

namespace fos
{

Conns_Handler::Conns_Handler(CommonTime* commonTime): commonTime(commonTime)
{
}

Conns_Handler::~Conns_Handler()
{
	delete commonTime;
}

std::vector<std::string> Conns_Handler::GetNames()
{
	std::vector<std::string> names;
	names.push_back("conns");
	return names;
}

void Conns_Handler::GetShortDescription(Messenger* messenger)
{
	messenger->write("prints current connections count.\n");
}

int Conns_Handler::Handle(Colonel::Nodes& nodes, Colonel::Requester* requester)
{
	requester->Listen(str(boost::format("currently there are %u connections.\n") % commonTime->GetCurrentConnectionsCount()));
	
	return Colonel::KEEP_ISSUING;
}

}//namespace fos


