
#ifndef FOS_BATCH_TIME_IMP_H
#define FOS_BATCH_TIME_IMP_H

#include "batch_time.h"

namespace fos
{

class Dispatcher;

class BatchTime_Imp: public BatchTime
{
public:
	
	BatchTime_Imp(Dispatcher* dispatcher);
	
#ifndef FOS_NO_DB
	db::ResponseRef DBRequest( char* query, const bool use, uint32_t size = 0 );
	db::ResponseRef DBRequest(const char* query);
	db::ResponseRef DBRequest(const std::string& query);
	db::ResponseRef DBRequest(const boost::format& query);
	void DBRequestA( char* query, const bool use, uint32_t size = 0 );
	void DBRequestA(const char* query);
	void DBRequestA(const std::string& query);
	void DBRequestA(const boost::format& query);
#endif//#ifndef FOS_NO_DB
	
	CommonTime* AllocateCommonTime();
	
private:
	
	Dispatcher* dispatcher;
};

}//namespace fos

#endif//#ifndef FOS_BATCH_TIME_IMP_H

