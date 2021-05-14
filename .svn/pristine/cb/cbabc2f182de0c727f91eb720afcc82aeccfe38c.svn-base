

#ifndef SEG_FAULT_HANDLER_H
#define SEG_FAULT_HANDLER_H

#include "../../colonel/colonel.h"

namespace fos
{

class SegFault_Handler: public Colonel::Handler
{
public:
	std::vector< std::string > GetNames();
	void GetShortDescription( Messenger* messenger );
	int Handle( Colonel::Nodes& nodes, Colonel::Requester* requester );
};

}//namespace fos

#endif//#ifndef SEG_FAULT_HANDLER_H

