

#ifndef FOS_CLIENTS_H_H
#define FOS_CLIENTS_H_H

#include "../../colonel/colonel.h"

#include "../../times/common_time.h"

namespace fos
{

class Clients_Handler: public Colonel::Handler
{
public:
	Clients_Handler(Dispatcher* dispatcher);
	
	std::vector<std::string> GetNames();
	void GetShortDescription(Messenger* messenger);
	int Handle(Colonel::Nodes& nodes, Colonel::Requester* requester);
	
	Dispatcher* dispatcher;
};

}//namespace fos

#endif//#ifndef FOS_CLIENTS_H_H

