
#include "dbstats_h.h"

#include <boost/format.hpp>

namespace fos
{

DBStats_Handler::DBStats_Handler(CommonTime* commonTime): commonTime(commonTime)
{
}

DBStats_Handler::~DBStats_Handler()
{
	delete commonTime;
}

std::vector<std::string> DBStats_Handler::GetNames()
{
	std::vector<std::string> names;
	names.push_back("db_stats");
	return names;
}

void DBStats_Handler::GetShortDescription(Messenger* messenger)
{
	messenger->write("prints database usage statistics.\n");
}

int DBStats_Handler::Handle(Colonel::Nodes& nodes, Colonel::Requester* requester)
{
	requester->Listen(str(boost::format("database usage statistics:\n%s\n") % commonTime->GetDBStats()));
	
	return Colonel::KEEP_ISSUING;
}

}//namespace fos


