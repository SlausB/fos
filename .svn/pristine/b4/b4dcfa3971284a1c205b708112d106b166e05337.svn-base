
#ifndef FOS_BATCH_REF_H
#define FOS_BATCH_REF_H

#include "batch_usage_map.h"

namespace fos
{

class Dispatcher;
class Batch;

typedef void(*DestructionHandler)(void* opaqueData);

/** Ссылка на данные. Служит как "scoped lock", освобождая логический мьютекс использования данных в деструкторе.*/
class BatchRef
{
	friend class Dispatcher;
	
public:
	
	/** Бессмысленный конструктор.*/
	BatchRef();
	
	/** Создавать ссылки на данные возможно только в framework'е. В пользовательском коде возможно только копирование.*/
	BatchRef(const BatchRef& batchRef);
	BatchRef& operator=(const BatchRef& batchRef);
	
	
	/** Освобождение мьютекса.*/
	~BatchRef();
	
	
	/** Сами данные.*/
	Batch* batch;
	
private:
	/** Мьютекс блокироваться НЕ будет.
	\param destructionHandler Вызывается при удалении последней ссылки с переданным opaqueData.*/
	BatchRef(Batch* batch, DestructionHandler destructionHandler, void* opaqueData);
	
	void Copy(const BatchRef& batchRef);
	
	/** true если текущая ссылка на данные является последней.*/
	bool last;
	
	DestructionHandler destructionHandler;
	void* opaqueData;
};

}//namespace fos

#endif//#ifndef FOS_BATCH_REF_H

