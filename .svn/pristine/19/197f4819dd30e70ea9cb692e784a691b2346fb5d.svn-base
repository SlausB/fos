
#include "seg_fault_h.h"

#include <boost/format.hpp>

#include <boost/thread.hpp>

std::vector<std::string> SegFault_Handler::GetNames()
{
	std::vector<std::string> names;
	names.push_back( "make_segmentation_fault" );
	return names;
}

void SegFault_Handler::GetShortDescription(Messenger* messenger)
{
	messenger->write( "simulates memory usage error which issues segmentation fault error\n" );
}

void make_fault()
{
	* ( ( int* ) ( 0 ) ) = 0;
}

int SegFault_Handler::Handle( Colonel::Nodes& nodes, Colonel::Requester* requester )
{
	//команда "make_segmentation_fault" без параметров:
	if ( nodes.size() > 0 )
	{
		requester->Listen( "E: make_segmentation_fault: command must NOT have any parameters.\n" );
		return Colonel::KEEP_ISSUING;
	}
	
	boost::format response( "simulating segmentation fault error...\n" );
	boost::thread deadThread( make_fault );
	
	return Colonel::KEEP_ISSUING;
}


