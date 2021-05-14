
#ifndef FOS_BATCH_USAGE_MAP_H
#define FOS_BATCH_USAGE_MAP_H

#include "sync/context_lock.h"

#include <deque>
#include <set>

#include "batch.h"

namespace fos
{

/** Значения Usage в std::map по ключу Key.*/
class BatchUsageMap
{

public:
	
	/** Использование данных.*/
	class Usage
	{
	public:
		
		/** 
		\param batchId Идентификатор данных, с которого будет взята копия для последующего удаления данных.
		*/
		Usage(Batch* batch, const bool underUsage, const BatchId* batchId);
		
		~Usage();
		
		/** Сами данные. Null если ещё не добавлялись.*/
		Batch* batch;
		
		/** true - данные сейчас используются.*/
		bool underUsage;
		
		/** Для возможности удаления данных при их освобождении от использования вследствие отключения последнего пользователя оных. Удаляется в деструкторе.*/
		BatchId* batchId;
		
		/** Ожидающие использования данных. Вызываются по мере освобождения данных по принципу FIFO. Здесь НЕ хранятся данные, так или иначе использующиеся в данный момент.*/
		std::deque<ContextLock*> requesters;
		
		/** Идентификаторы сессий клиентов, использующих текущие данные.*/
		std::set<uint64_t> users;
	};
	
	/** Для хранения Usage в std::map. Создаётся копия идентификатора, которая удаляется в деструкторе.*/
	class Key
	{
	public:
		Key(const BatchId* batchId, const char copyingType);
		Key(const Key& key);
		/** Удаление идентификатора.*/
		~Key();
		
		BatchId* batchId;
		
		/** Как могут копироваться данные.*/
		enum
		{
			COPY,	/**< Должны быть скопированы в конструкторе. В деструкторе будут удаляться.*/
			SHARED,	/**< Используется указатель, переданный в конструкторе. В деструкторе удаляться НЕ будут.*/
		};
		/** Как скопированы данные.*/
		char copyType;
	};
	
	/** Для хранения Usage в std::map - хранение указателя на Usage, местоположение которого в памяти может меняться при добавлении элементов в std::map.*/
	class Holder
	{
	public:
		
		Holder();
		Holder(const Holder& holder);
		Holder(Usage* usage);
		
		/** NULL если ещё не существует.*/
		Usage* usage;
	};
	
private:
	
	/** Сравнение ключей batch в std::map.*/
	class Comparator
	{
	public:
		bool operator()(const Key& left, const Key& right) const;
	};
	
	
public:
	
	std::map<Key, Holder, Comparator> batches;
	
	typedef std::map<Key, Holder, Comparator>::iterator Iterator;
};

}//namespace fos

#endif//#ifndef FOS_BATCH_USAGE_MAP_H

