
#ifndef CONSOLE_REQUESTER_H
#define CONSOLE_REQUESTER_H

#include "../../colonel/colonel.h"
#include "../../output/messenger.h"

#include <boost/thread.hpp>

namespace fos
{

class ConsoleRequester: Colonel::Requester
{
public:
	
	/** Конструктор - начало считывания с консоли вводимых команд.
	\param colonel Куда будут передаваться команды.*/
	ConsoleRequester(Colonel* colonel, Messenger* messenger, ContextLock* contextLock);
	
	virtual void Listen(const std::string& result);

private:

	Messenger* messenger;
};

}//namespace fos


#endif//#ifndef CONSOLE_REQUESTER_H

