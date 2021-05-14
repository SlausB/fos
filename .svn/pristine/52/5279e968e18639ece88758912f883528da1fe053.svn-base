

#ifndef FOS_CONNS_HANDLER_H
#define FOS_CONNS_HANDLER_H

#include "../../colonel/colonel.h"

#include "../../times/common_time.h"

namespace fos
{

class Conns_Handler: public Colonel::Handler
{
public:
	Conns_Handler(CommonTime* commonTime);
	
	~Conns_Handler();
	
	std::vector<std::string> GetNames();
	void GetShortDescription(Messenger* messenger);
	int Handle(Colonel::Nodes& nodes, Colonel::Requester* requester);
	
	CommonTime* commonTime;
};

}//namespace fos

#endif//#ifndef FOS_CONNS_HANDLER_H

