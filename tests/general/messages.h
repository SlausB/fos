
#ifndef MESSAGES_H
#define MESSAGES_H

/** Типы входящих сообщений.*/
class ClientToServer
{
public:
	enum
	{
		HELLO,
		GET_VERY_BIG_MESSAGE,
		GET_VERY_LONG_MESSAGE,
		LOGIN,	/**< (std::string name) */
		GET_BUILDINGS,
		PUT_BUILDING,	/**< (int type) */
		DISCONNECT_ME,
		PING_ME,
		MEM_LEAK_DETECT,	/**< (8000 bytes) */
		GET_MEM_USAGE,
		DROP_BUILDINGS,
		LONG_GET_BUILDINGS,
		LONG_PUT_BUILDING,	/**< (int type) */
		LONG_DROP_BUILDINGS,
		USE_BATCH,	/**< (int id) */
		DUMP_MEMORY_LEAKS,
	};
};

/** Типы исходящих сообщений.*/
class ServerToClient
{
public:
	enum
	{
		HELLO,
		VERY_BIG_MESSAGE,
		VERY_LONG_MESSAGE,
		BUILDINGS,	/**< (int[] types) */
		PING,
		MEM_LEAK_DETECT_RESULT,	/**< (int result) */
		MEM_USAGE,	/**< (int mem_usage_in_bytes) */
		LONG_BUILDINGS,	/**< (int[] types) */
	};
};

#endif//#ifndef MESSAGES_H

