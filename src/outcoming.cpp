
#include "outcoming.h"

#include "memdbg_start.h"

namespace fos
{

Outcoming::Outcoming()
{
}

Outcoming::Outcoming(const std::string& data): data(data)
{
}

Outcoming::Outcoming(const uint64_t& sessionId)
{
	recipients.push_back(sessionId);
}

Outcoming::Outcoming(const std::string& data, const uint64_t& sessionId): data(data)
{
	recipients.push_back(sessionId);
}

Outcoming::Outcoming(const uint64_t& sessionId, const std::string& data): data(data)
{
	recipients.push_back(sessionId);
}

void Outcoming::SetData(const std::string& data)
{
	this->data = data;
}

void Outcoming::AddRecipient(const uint64_t& sessionId)
{
	recipients.push_back(sessionId);
}

}//namespace fos

#include "memdbg_end.h"

