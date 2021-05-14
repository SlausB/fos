

#ifndef FOS_CLIENT_H
#define FOS_CLIENT_H

namespace fos
{

class Batch;

/** Обработка входящих событий для клиента с одним идентификатором сессии.*/
class Client
{
public:
	
	Client(const char type, const uint64_t& sessionId, bool isOnHandle, Batch* luggage, const std::string& ipAddress);
	
	uint64_t sessionId;
	
	class Event
	{
	public:
		
		Event(const char type);
		/** Удаление данных в зависимости от типа сообщения.*/
		~Event();
		
		/** Каких типов бывают события.*/
		enum
		{
			CONNECTION,
			MESSAGE,
			DISCONNECTION,
			DROP_BATCH,
		};
		/** Тип текущего сообщения.*/
		char type;
		
		char* messageData;
		size_t messageLength;
		std::string* ipAddress;
		BatchId* batchId;
	};
	
	/** Входящие сообщения, ожидающие очереди обработки.*/
	std::deque<Event*> events;
	
	/** Создаётся при первом событии от клиента.*/
	Batch* luggage;
	
	/** true если в данный момент в каком-то потоке обрабатывается событие.*/
	bool isOnHandle;
	
	/** Данные, которые текущий клиент использует (когда-либо запрашивал и ещё не освободил).*/
	BatchUsageMap batchesUnderUsage;
	
	/** Типы клиентов.*/
	enum
	{
		ORDINARY,	/**< Обычный клиент, подключившийся извне.*/
		IMMORTAL,	/**< Клиент, эмулируйщий использование данных со стороны диспетчера. При использовании такого клиента используется мьютекс.*/
	};
	const char type;
	
	/** Для вывода информации о клиенте в консоли.*/
	std::string ipAddress;
};

}//namespace fos

#endif//#ifndef FOS_CLIENT_H

