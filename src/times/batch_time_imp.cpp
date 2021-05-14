
#include "batch_time_imp.h"

#include "../dispatcher.h"

#include "../memdbg_start.h"

namespace fos
{

BatchTime_Imp::BatchTime_Imp(Dispatcher* dispatcher): dispatcher(dispatcher)
{
}

#ifndef FOS_NO_DB

db::ResponseRef BatchTime_Imp::DBRequest( char* query, const bool use, uint32_t size )
{
	return dispatcher->DBRequest( query, use, size );
}

db::ResponseRef BatchTime_Imp::DBRequest(const char* query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef BatchTime_Imp::DBRequest(const std::string& query)
{
	return dispatcher->DBRequest(query);
}

db::ResponseRef BatchTime_Imp::DBRequest(const boost::format& query)
{
	return dispatcher->DBRequest(query);
}

void BatchTime_Imp::DBRequestA( char* query, const bool use, uint32_t size )
{
	dispatcher->DBRequestA( query, use, size );
}

void BatchTime_Imp::DBRequestA(const char* query)
{
	dispatcher->DBRequestA(query);
}

void BatchTime_Imp::DBRequestA(const std::string& query)
{
	dispatcher->DBRequestA(query);
}

void BatchTime_Imp::DBRequestA(const boost::format& query)
{
	dispatcher->DBRequestA(query);
}

#endif//#ifndef FOS_NO_DB

CommonTime* BatchTime_Imp::AllocateCommonTime()
{
	return dispatcher->AllocateCommonTime();
}

}//namespace fos

#include "../memdbg_end.h"

