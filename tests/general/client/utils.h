
#ifndef UTILS_H
#define UTILS_H

#include <boost/asio.hpp>

namespace utils
{

/** Возвращает true если всё прочитано.*/
bool ReadSome(std::string& strBuf, boost::asio::ip::tcp::socket& socket, bool& error);

/** Читать полностью сообщение в указанный буфер.*/
void Read(std::string& strBuf, boost::asio::ip::tcp::socket& socket, bool& error);

}


#define READ(argument) try { argument; } catch(std::exception& e) { printf("E: reading failed with exception \"%s\". Returning.\n", e.what()); }

#endif//#ifndef UTILS_H



