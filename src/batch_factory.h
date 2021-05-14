
#ifndef FOS_BATCH_FACTORY_H
#define FOS_BATCH_FACTORY_H

//vc2008 does not have <stdint.h>:
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#include <boost/cstdint.hpp>
using boost::int64_t;
using boost::uint64_t;
#else
#include <stdint.h>
#endif

namespace fos
{

class Batch;
class BatchId;

/** Механизм создания данных. Передаётся из пользовательской части при создании диспетчера.*/
class BatchFactory
{
public:
	/** Должно создавать данные на основе идентификатора оных.
	Пример:
	\code
	Batch* MyBatchFactory::CreateCustomBatch(const BatchId* batchId)
	{
		if(batchId->GetType().compare("NumericalBatchId") == 0)
		{
			MyBatch* myBatch = new MyBatch;
			
			NumericalBalchId* numericalBatchId = (NumericalBatchId*)batchId;

			//можно как-нибудь заполнить данные на основе данных идентификатора - возможно использовать синхронные запросы в базу данных...

			return myBatch;
		}
		else
		{
			printf("unknown batch id\n");
		}

		return NULL;
	}
	\endcode

	Функция вызывается из множества потоков.
	*/
	virtual Batch* CreateCustomBatch(const BatchId* batchId) = 0;
	
	/** Должно создавать данные на основе идентификатора соединения с клиентом. Эти данные передаются с каждым сообщением от клиента.
	В OnCreation() созданных данных передаётся NumericalBatchId с идентификатором сессии клиента.*/
	virtual Batch* CreateLuggageBatch(const uint64_t& sessionId) = 0;
};

}//namespace fos


#endif//#ifndef FOS_BATCH_FACTORY_H


