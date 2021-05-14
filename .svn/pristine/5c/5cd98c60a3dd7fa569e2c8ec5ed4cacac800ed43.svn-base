
#include "batch_usage_map.h"

#include "memdbg_start.h"


namespace fos
{


BatchUsageMap::Usage::Usage(Batch* batch, const bool underUsage, const BatchId* batchId): batch(batch), underUsage(underUsage), batchId(batchId->MakeCopy())
{
}

BatchUsageMap::Usage::~Usage()
{
	delete batch;
	delete batchId;
}


BatchUsageMap::Key::Key(const BatchId* batchId, const char copyType): copyType(copyType)
{
	if(copyType == BatchUsageMap::Key::SHARED)
	{
		this->batchId = const_cast<BatchId*>(batchId);
	}
	else
	{
		this->batchId = batchId->MakeCopy();
	}
}

BatchUsageMap::Key::Key(const Key& key): batchId(key.batchId->MakeCopy()), copyType(BatchUsageMap::Key::COPY)
{
}

BatchUsageMap::Key::~Key()
{
	if(copyType == BatchUsageMap::Key::COPY)
	{
		delete batchId;
	}
}


BatchUsageMap::Holder::Holder(): usage(NULL)
{
}

BatchUsageMap::Holder::Holder(const Holder& holder): usage(holder.usage)
{
}

BatchUsageMap::Holder::Holder(Usage* usage): usage(usage)
{
}



bool BatchUsageMap::Comparator::operator()(const BatchUsageMap::Key& left, const BatchUsageMap::Key& right) const
{
	const char leftType = left.batchId->GetType();
	const char rightType = right.batchId->GetType();
	if ( leftType < rightType )
	{
		return true;
	}
	else if ( leftType > rightType )
	{
		return false;
	}
	//equal:
	
	if ( leftType == BatchId::INTEGRAL )
	{
		IntegralBatchId* leftIntegral = ( IntegralBatchId* ) left.batchId;
		IntegralBatchId* rightIntegral = ( IntegralBatchId* ) right.batchId;
		return leftIntegral->GetValue() < rightIntegral->GetValue();
	}
	else if ( leftType == BatchId::INTEGRAL_ARRAY )
	{
		std::vector< uint64_t > leftArray = ( ( IntegralArrayBatchId* ) left.batchId )->GetValues();
		std::vector< uint64_t > rightArray = ( ( IntegralArrayBatchId* ) right.batchId )->GetValues();
		const size_t leftSize = leftArray.size();
		const size_t rightSize = rightArray.size();
		if ( leftSize < rightSize )
		{
			return true;
		}
		else if ( leftSize > rightSize )
		{
			return false;
		}
		for ( size_t i = 0; i < leftSize; ++i )
		{
			if ( leftArray[ i ] < rightArray[ i ] )
			{
				return true;
			}
			else if ( leftArray[ i ] > rightArray[ i ] )
			{
				return false;
			}
		}
		return false;
	}
	else if ( leftType == BatchId::LITERAL )
	{
		return ( ( LiteralBatchId* ) left.batchId )->GetValue() < ( ( LiteralBatchId* ) right.batchId )->GetValue();
	}
	else if ( leftType == BatchId::LITERAL_ARRAY )
	{
		std::vector<std::string> leftArray = ( ( LiteralArrayBatchId* ) left.batchId )->GetValues();
		std::vector<std::string> rightArray = ((LiteralArrayBatchId*)right.batchId)->GetValues();
		const size_t leftSize = leftArray.size();
		const size_t rightSize = rightArray.size();
		if ( leftSize < rightSize )
		{
			return true;
		}
		else if ( leftSize > rightSize )
		{
			return false;
		}
		for ( size_t i = 0; i < leftSize; ++i )
		{
			if ( leftArray[ i ] < rightArray[ i ] )
			{
				return true;
			}
			else if ( rightArray[ i ] < leftArray[ i ] )
			{
				return false;
			}
		}
		return false;
	}
	//some custom data identificator type:
	else
	{
		return left.batchId->Compare( right.batchId );
	}
	//never reach here:
	return true;
}

}//namespace fos

#include "memdbg_end.h"

