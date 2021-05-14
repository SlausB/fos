
#include "batch_id_imp.h"

#include "memdbg_start.h"


namespace fos
{

/*uint64_t my_atoll(const char *instr)
{
	uint64_t retval;
	
	retval = 0;
	for (; *instr; instr++)
	{
		const int summand = (*instr - '0');
		if(summand < 0 || summand > 9)
		{
			return 0;
		}
		retval = 10*retval + summand;
	}
	return retval;
}*/


IntegralBatchId::IntegralBatchId(const IntegralBatchId& batchId): value(batchId.value)
{
}

IntegralBatchId::IntegralBatchId(const IntegralBatchId* batchId): value(batchId->value)
{
}

IntegralBatchId::IntegralBatchId(const uint64_t& value): value(value)
{
}

IntegralBatchId::IntegralBatchId(const int value): value(value)
{
}

IntegralBatchId& IntegralBatchId::operator=(const IntegralBatchId& integralBatchId)
{
	value = integralBatchId.value;
	return *this;
}

char IntegralBatchId::GetType() const
{
	return BatchId::INTEGRAL;
}

BatchId* IntegralBatchId::MakeCopy() const
{
	return new IntegralBatchId(this);
}

const uint64_t& IntegralBatchId::GetValue() const
{
	return value;
}


IntegralArrayBatchId::IntegralArrayBatchId(const IntegralArrayBatchId& integralArrayBatchId): values(integralArrayBatchId.values)
{
}

IntegralArrayBatchId::IntegralArrayBatchId(const IntegralArrayBatchId* integralArrayBatchId): values(integralArrayBatchId->values)
{
}

IntegralArrayBatchId::IntegralArrayBatchId(const uint64_t& value)
{
	values.push_back(value);
}

IntegralArrayBatchId::IntegralArrayBatchId(int value)
{
	values.push_back(value);
}

IntegralArrayBatchId::IntegralArrayBatchId(const std::vector<int>& values)
{
	for(std::vector<int>::const_iterator it = values.begin(); it != values.end(); it++)
	{
		this->values.push_back(*it);
	}
}

IntegralArrayBatchId::IntegralArrayBatchId(const std::vector<uint64_t>& values): values(values)
{
}

void IntegralArrayBatchId::Push(const uint64_t& value)
{
	values.push_back(value);
}

void IntegralArrayBatchId::Push(int value)
{
	values.push_back(value);
}

void IntegralArrayBatchId::Push(const std::vector<int>& values)
{
	for(std::vector<int>::const_iterator it = values.begin(); it != values.end(); it++)
	{
		this->values.push_back(*it);
	}
}

void IntegralArrayBatchId::Push(const std::vector<uint64_t>& values)
{
	for(std::vector<uint64_t>::const_iterator it = values.begin(); it != values.end(); it++)
	{
		this->values.push_back(*it);
	}
}

IntegralArrayBatchId& IntegralArrayBatchId::operator=(const IntegralArrayBatchId& integralArrayBatchId)
{
	values = integralArrayBatchId.values;
	return *this;
}

char IntegralArrayBatchId::GetType() const
{
	return BatchId::INTEGRAL_ARRAY;
}

BatchId* IntegralArrayBatchId::MakeCopy() const
{
	return new IntegralArrayBatchId(this);
}

size_t IntegralArrayBatchId::GetSize() const
{
	return values.size();
}

const std::vector<uint64_t>& IntegralArrayBatchId::GetValues() const
{
	return values;
}

uint64_t IntegralArrayBatchId::GetValue(const size_t index) const
{
	if(index >= values.size())
	{
		return 0;
	}
	
	return values[index];
}


LiteralBatchId::LiteralBatchId(const LiteralBatchId& batchId): value(batchId.value)
{
}

LiteralBatchId::LiteralBatchId(const LiteralBatchId* batchId): value(batchId->value)
{
}

LiteralBatchId::LiteralBatchId(const std::string& value): value(value)
{
}

LiteralBatchId::LiteralBatchId(const char* value): value(value)
{
}

LiteralBatchId& LiteralBatchId::operator=(const LiteralBatchId& literalBatchId)
{
	value = literalBatchId.value;
	return *this;
}

char LiteralBatchId::GetType() const
{
	return BatchId::LITERAL;
}

BatchId* LiteralBatchId::MakeCopy() const
{
	return new LiteralBatchId(this);
}

const std::string& LiteralBatchId::GetValue() const
{
	return value;
}


LiteralArrayBatchId::LiteralArrayBatchId(const LiteralArrayBatchId& literalArrayBatchId): values(literalArrayBatchId.values)
{
}

LiteralArrayBatchId::LiteralArrayBatchId(const LiteralArrayBatchId* literalArrayBatchId): values(literalArrayBatchId->values)
{
}

LiteralArrayBatchId::LiteralArrayBatchId(const std::string& value)
{
	values.push_back(value);
}

LiteralArrayBatchId::LiteralArrayBatchId(const char* value)
{
	values.push_back(value);
}

LiteralArrayBatchId& LiteralArrayBatchId::operator=(const LiteralArrayBatchId& batchId)
{
	values = batchId.values;
	return *this;
}

char LiteralArrayBatchId::GetType() const
{
	return BatchId::LITERAL_ARRAY;
}

BatchId* LiteralArrayBatchId::MakeCopy() const
{
	return new LiteralArrayBatchId(this);
}

size_t LiteralArrayBatchId::GetSize() const
{
	return values.size();
}

const std::vector<std::string>& LiteralArrayBatchId::GetValues() const
{
	return values;
}

const std::string& LiteralArrayBatchId::GetValue(const size_t index) const
{
	return values[index];
}

}//namespace fos

#include "memdbg_end.h"


