

#ifndef FOS_DBSTATS_HANDLER_H
#define FOS_DBSTATS_HANDLER_H

#include "../../colonel/colonel.h"

#include "../../times/common_time.h"

namespace fos
{

class DBStats_Handler: public Colonel::Handler
{
public:
	DBStats_Handler(CommonTime* commonTime);
	
	~DBStats_Handler();
	
	std::vector<std::string> GetNames();
	void GetShortDescription(Messenger* messenger);
	int Handle(Colonel::Nodes& nodes, Colonel::Requester* requester);
	
	CommonTime* commonTime;
};

}//namespace fos

#endif//#ifndef FOS_DBSTATS_HANDLER_H

