﻿

#ifndef FOS_BATCH_H
#define FOS_BATCH_H

#include <string>

namespace fos
{

class BatchId;
class BatchTime;

/** Набор данных, передающийся от сервера серверу.
Новый набор данных можно создать только в результате десериализации или внутри переданной фабрики на серверной части. Обязательное задание полиморфного идентификатора.*/
class Batch
{
public:
	
	/** Чтобы данные вообще удалялись из памяти.*/
	virtual ~Batch();
	
	/** Вызывается после создания данных. Возможно осуществление синхронных запросов в базу данных.*/
	virtual void OnCreation(const BatchId* batchId, BatchTime* batchTime) = 0;
	
	/** Вызывается перед удалением данных при отключении всех пользователей данных. Возможно осуществление синхронных запросов в базу данных.*/
	virtual void OnDestruction(BatchTime* batchTime) = 0;
	
	/** Используется в консольных командах для вывода описания данных.*/
	virtual std::string Describe();
};

/** Идентификатор данных. Определяется реализацией. Может копироваться, поэтому нужно определять конструктор копирования если комирование нетривиально.*/
class BatchId
{
public:
	
	/** Чтобы идентификатор вообще удалялся из памяти.*/
	virtual ~BatchId();
	
	/** Для обеспечения упорядоченности идентификаторов разных типов и значений в одном хэше.*/
	virtual char GetType() const = 0;
	
	/** Сделать идентичную копию этого идентификатора данных. Необходимо для нужд реализации сервера.*/
	virtual BatchId* MakeCopy() const = 0;
	
	/** Вызывается в случае если типы двух сравниваемых идентификаторов данных идентичны и только для НЕстандартных идентификаторов (начинающихся с FIRST_CUSTOM_BATCH_ID).
	\param batchId Идентификатора данных, с которым нужно сравнить. Тип (то есть GetType()) этого идентификатора всегда будет таким же, что и у самого объекта, у которого вызвана функция, поэтому можно смело кастовать к такому же классу (если типы были указаны верно).
	\return true если BatchId, у которого вызвана эта функция, "меньше" передаваемого; false в противном случае.*/
	virtual bool Compare(const BatchId* batchId) const;
	
	/** Каких типов бывают идентификаторы данных.*/
	enum
	{
		INTEGRAL,
		INTEGRAL_ARRAY,
		LITERAL,
		LITERAL_ARRAY,
		FIRST_CUSTOM_BATCH_ID,	/**< Первый свободный идентификатор типа, который может быть использован для собственных BatchId.*/
	};
};

}//namespace fos


#endif//#ifndef FOS_BATCH_H

