

#include "batch.h"

#include "memdbg_start.h"

using namespace fos;

std::string Batch::Describe()
{
	return "[no description provided. Reimplement \"std::string Describe()\" function.]";
}

Batch::~Batch()
{
}

BatchId::~BatchId()
{
}

bool BatchId::Compare(const BatchId* batchId) const
{
	return true;
}

#include "memdbg_end.h"
