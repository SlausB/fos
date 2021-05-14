

#ifndef OUTCOMING_H
#define OUTCOMING_H

#include <string>
#include <vector>
#include <boost/cstdint.hpp>

namespace fos
{

class ClientsHandler;

/** Исходящее сообщение для отправки.*/
class Outcoming
{
	friend class ClientsHandler_Asio;
	friend class ClientsHandler_Realtime;
	friend class ClientsHandler_Strand;
	friend class ClientsHandler_Aio;
	
public:
	
	/** Данные и получателя (-ей) нужно будет установить позже.*/
	Outcoming();
	
	/** Получателя нужно будет установить позже.*/
	Outcoming(const std::string& data);
	
	/** Данные нужно будет установить позже.*/
	Outcoming(const uint64_t& sessionId);
	
	Outcoming(const std::string& data, const uint64_t& sessionId);
	Outcoming(const uint64_t& sessionId, const std::string& data);
	
	/** Установка данных для передачи. Какие-либо предыдуще установленные данные будут проигнорированы.*/
	void SetData(const std::string& data);
	
	
	/** Добавить получателя.*/
	void AddRecipient(const uint64_t& sessionId);
	
private:
	
	/** Указанные для передачи данные.*/
	std::string data;
	
	/** Получатели сообщения.*/
	std::vector<uint64_t> recipients;
};

}//namespace fos


#endif//ifndef OUTCOMING_H

