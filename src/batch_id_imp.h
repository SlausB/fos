
/** \file Некоторые стандартные идентификаторы данных.*/

#ifndef FOS_BATCH_ID_IMP_H
#define FOS_BATCH_ID_IMP_H

#include "batch.h"

//vc2008 does not have <stdint.h>:
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#include <boost/cstdint.hpp>
using boost::int32_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;
#else
#include <stdint.h>
#endif

#include <boost/format.hpp>

namespace fos
{

/** Единственное 64-битное целочисленное значение как идентификатор данных.*/
class IntegralBatchId: public BatchId
{
public:
	IntegralBatchId(const IntegralBatchId& batchId);
	IntegralBatchId(const IntegralBatchId* batchId);
	IntegralBatchId(const uint64_t& value);
	IntegralBatchId(const int value);

	IntegralBatchId& operator=(const IntegralBatchId& integralBatchId);

	char GetType() const;
	
	BatchId* MakeCopy() const;
	
	const uint64_t& GetValue() const;

private:
	
	uint64_t value;
};

/** Массив 64-битных целочисленных значений как идентификатор данных.*/
class IntegralArrayBatchId: public BatchId
{
public:

	IntegralArrayBatchId(const IntegralArrayBatchId& integralArrayBatchId);
	IntegralArrayBatchId(const IntegralArrayBatchId* integralArrayBatchId);
	IntegralArrayBatchId(const uint64_t& value);
	IntegralArrayBatchId(int value);
	IntegralArrayBatchId(const std::vector<int>& values);
	IntegralArrayBatchId(const std::vector<uint64_t>& values);

	void Push(const uint64_t& value);
	void Push(int value);
	void Push(const std::vector<int>& values);
	void Push(const std::vector<uint64_t>& values);

	IntegralArrayBatchId& operator=(const IntegralArrayBatchId& integralArrayBatchId);
	
	char GetType() const;
	
	BatchId* MakeCopy() const;
	
	size_t GetSize() const;
	const std::vector<uint64_t>& GetValues() const;
	/** Если элемент под указанным индексом отсутствует, то возвращает 0.*/
	uint64_t GetValue(const size_t index) const;

private:
	
	std::vector<uint64_t> values;
};

/** Строковый идентификатор данных.*/
class LiteralBatchId: public BatchId
{
public:
	LiteralBatchId(const LiteralBatchId& batchId);
	LiteralBatchId(const LiteralBatchId* batchId);
	LiteralBatchId(const std::string& value);
	LiteralBatchId(const char* value);
	LiteralBatchId& operator=(const LiteralBatchId& literalBatchId);
	
	char GetType() const;
	
	BatchId* MakeCopy() const;
	
	const std::string& GetValue() const;

private:
	
	std::string value;
};

/** Массив строк как идентификатор данных.*/
class LiteralArrayBatchId: public BatchId
{
public:
	LiteralArrayBatchId(const LiteralArrayBatchId& batchId);
	LiteralArrayBatchId(const LiteralArrayBatchId* batchId);
	LiteralArrayBatchId(const std::string& value);
	LiteralArrayBatchId(const char* value);
	LiteralArrayBatchId& operator=(const LiteralArrayBatchId& batchId);
	
	char GetType() const;
	
	BatchId* MakeCopy() const;
	
	size_t GetSize() const;
	const std::vector<std::string>& GetValues() const;
	const std::string& GetValue(const size_t index) const;
	
private:
	
	std::vector<std::string> values;
};

}//namespace fos

#endif//#ifndef FOS_BATCH_ID_IMP_H

