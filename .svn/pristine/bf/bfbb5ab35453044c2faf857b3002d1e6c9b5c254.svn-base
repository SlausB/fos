
#include "exit_h.h"

#include <boost/format.hpp>

std::vector<std::string> Exit_Handler::GetNames()
{
	std::vector<std::string> names;
	names.push_back("exit");
	return names;
}

void Exit_Handler::GetShortDescription(Messenger* messenger)
{
	messenger->write("exits the program.\n");
}

int Exit_Handler::Handle(Colonel::Nodes& nodes, Colonel::Requester* requester)
{
	//команда "exit" без параметров:
	if(nodes.size() > 0)
	{
		requester->Listen("E: exit: command must NOT have any parameters.\n");
		return Colonel::KEEP_ISSUING;
	}
	
	boost::format response("exiting...\n");
	requester->Listen(response.str());
	
	//...
	
	requester->Listen("exited.\n");
	
	return Colonel::EXIT;
}


