

#ifndef EXIT_HANDLER_H
#define EXIT_HANDLER_H

#include "../../colonel/colonel.h"

namespace fos
{

class Exit_Handler: public Colonel::Handler
{
public:
	std::vector<std::string> GetNames();
	void GetShortDescription(Messenger* messenger);
	int Handle(Colonel::Nodes& nodes, Colonel::Requester* requester);
};

}//namespace fos

#endif//#ifndef EXIT_HANDLER_H

