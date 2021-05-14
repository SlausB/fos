
#include "utils.h"

namespace utils
{

/** Возвращает true если всё прочитано.*/
bool ReadSome(std::string& strBuf, boost::asio::ip::tcp::socket& socket, bool& error)
{
	const int BUF_SIZE = 128;
	char charBuf[BUF_SIZE];

	error = false;
	
	//socket::read_some() иногда возвращает FFFFFFFF значение (-1), поэтому используем int:
	int readBytes;
	try
	{
		readBytes = socket.read_some(boost::asio::buffer(charBuf, BUF_SIZE));
	}
	catch(std::exception& e)
	{
		error = true;
	}
	if(readBytes >= 0 && readBytes <= BUF_SIZE)
	{
		strBuf.append(charBuf, readBytes);
	}

	//если буфер был полностью занят, то значит ещё есть что читать:
	return readBytes < BUF_SIZE;
}

/** Читать полностью сообщение в указанный буфер.*/
void Read(std::string& strBuf, boost::asio::ip::tcp::socket& socket, bool& error)
{
	for(;;)
	{
		const bool allRead = ReadSome(strBuf, socket, error);
		if(error)
		{
			break;
		}
		if(allRead)
		{
			break;
		}
	}
}

}