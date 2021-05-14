
/** \file Реализация соединения с клиентами на основе библиотеки asio с использованием.*/

#ifndef FOS_CLIENTS_HANDLER_ASIO_H
#define FOS_CLIENTS_HANDLER_ASIO_H

#include <string>
#include <map>

#include <boost/cstdint.hpp>

#include <boost/thread.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "../clients_handler.h"

#include "../meter/transmitter.h"

#include "../../meters_factory.h"

#include "asio.hpp"


namespace fos
{

class ClientsConnIFace;
class Outcoming;

/** Server for communication with clients base on ASIO.*/
class ClientsHandler_Asio: public ClientsHandler
{
public:
	
	ClientsHandler_Asio( MetersFactory* metersFactory, const short port, ClientsConnIFace* clientsConnIFace );
	
	~ClientsHandler_Asio();
	
	
	/** D->this. Messages sending.*/
	void Send( const Outcoming& outcoming );
	void Send( const uint64_t& sessionId, const OutStream& outStream );
	void Send( const std::vector< uint64_t >& ids, const OutStream& outStream );
	void Send( const uint64_t& sessionId, const std::string& data );
	void Send( const uint64_t& sessionId, const void* data, const size_t length );
	/** D->this. Removing clients with specified id.*/
	void Disconnect( const uint64_t& id );
	void Disconnect( const std::vector< uint64_t >& ids );
	/** D->this. Returns list of currently connected clients.*/
	boost::shared_ptr< std::vector< uint64_t > > GetIds();
	
	/** Interrupting of accepting incoming connections - stopping that process if it going on within some different thread.*/
	void CancelAcceptingConnections();
	/** Disconnecting all currently connected clients with "disconnected" message for dispatcher.*/
	void DisconnectClients();
	
private:
	
	class Client
	{
	public:
		
		Client( Collector* collector, asio::io_service& io_service );
		
		~Client();
		
		
		asio::ip::tcp::socket socket;
		
		static const int BUFFER_SIZE = 8192;
		/** Where incoming messages are read.*/
		char readBuffer[ BUFFER_SIZE ];
		
		uint64_t sessionId;
		
		Collector* collector;
	};
	
	void Run();
	
	void handle_accept( boost::shared_ptr< Client >& client, const asio::error_code& error );
	
	void DispatchedSend_Single( const uint64_t sessionId, boost::shared_ptr< Transmitter >& transmitter );
	void DispatchedSend_Array( const std::vector< uint64_t > ids, boost::shared_ptr< Transmitter >& transmitter );
	void DispatchedDisconnect_Single( const uint64_t sessionId );
	void DispatchedDisconnect_Array( const std::vector< uint64_t > ids );
	
	void handle_read( boost::shared_ptr< Client >& client, const asio::error_code& error, const size_t bytes_transferred );
	void handle_write( boost::shared_ptr< Client >& client, boost::shared_ptr< Transmitter >& transmitter, const asio::error_code& error, const size_t bytes_transferred );
	
	/** Disconnect and remove client.*/
	void Drop( boost::shared_ptr< Client >& client, const bool printWarning );
	
	/** Thread-UNsafe client disconnection.*/
	void Disconnect_TU( const uint64_t& sessionId );
	
	void DispatchedCancelAcceptingConnections();
	
	//where incoming connections will appear:
	asio::io_service io_service;
	asio::ip::tcp::acceptor* acceptor;
	asio::strand strand;
	
	ClientsConnIFace* clientsConnIFace;
	
	/** All current connections.*/
	std::map< uint64_t, boost::shared_ptr< Client > > clients;
	
	/** Connections usage.*/
	boost::mutex clientsMutex;
	
	ContextLock* cancellingAcceptingConnectionsContextLock;
	/** true - right now aborting accepting incoming connections is doing.*/
	bool cancelling;
	
	/** Accepted from outside receiving/transmitting policy creation factory.*/
	MetersFactory* metersFactory;
};

}//namespace fos

#endif//#ifndef FOS_CLIENTS_HANDLER_ASIO_H

