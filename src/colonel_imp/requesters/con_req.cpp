
#include "con_req.h"

/** Потоковая функция чтения команд с консоли.*/
void Reader(ConsoleRequester* conReq, Colonel* colonel, Messenger* messenger, ContextLock* contextLock)
{
	messenger->info("I: Console requester is ready for issuing commands...\n");
	
READ:
	
	std::string command;
	
	char character;
	while((character = getchar()) != '\n')
	{
		command.push_back(character);
	}
	
	switch(colonel->Execute(command, (Colonel::Requester*)conReq))
	{
	case Colonel::EXIT:
		contextLock->Resume();
		return;
		break;
	}
	
	goto READ;
}

ConsoleRequester::ConsoleRequester(Colonel* colonel, Messenger* messenger, ContextLock* contextLock): messenger(messenger)
{
	//этот объект создали в общем коде инициализации, поэтому отпочковываемся для вечного цикла чтения команд с консоли:
	boost::thread readingThread(Reader, this, colonel, messenger, contextLock);
}

void ConsoleRequester::Listen(const std::string& result)
{
	messenger->write(result);
}

