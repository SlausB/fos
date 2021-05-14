
#include "clients_handler_asio.h"

#include "../../memdbg_start.h"

#include "../../clients_conn_iface.h"

#include "../meter/incoming_data_handler.h"

//#include "../meter/leb128_transmitter.h"
#include "../meter/simple_transmitter.h"



namespace fos
{



class AsioIncomingDataHandler: public IncomingDataHandler
{
public:
	AsioIncomingDataHandler( const uint64_t& sessionId, ClientsConnIFace* clientsConnIFace ): sessionId( sessionId ), clientsConnIFace( clientsConnIFace )
	{
	}
	
	void HandleMessage( const size_t length, const void* buffer )
	{
		clientsConnIFace->HandleMessage( sessionId, length, ( const char* ) buffer );
	}

private:
	uint64_t sessionId;
	ClientsConnIFace* clientsConnIFace;
};



ClientsHandler_Asio::Client::Client( Collector* collector, asio::io_service& io_service ): collector( collector ), socket( io_service )
{
}

ClientsHandler_Asio::Client::~Client()
{
	delete collector;
}



ClientsHandler_Asio::ClientsHandler_Asio( MetersFactory* metersFactory, const short port, ClientsConnIFace* clientsConnIFace ):
	metersFactory( metersFactory),
	strand( io_service ),
	clientsConnIFace( clientsConnIFace ),
	cancelling( false ),
	acceptor ( NULL )
{
	try
	{
		acceptor = new asio::ip::tcp::acceptor( io_service, asio::ip::tcp::endpoint( asio::ip::tcp::v4(), port ) );
	}
	catch( std::exception& e )
	{
		Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::ClientsHandler_Asio(): listening failed with error \"%s\".\n" ) % e.what() );
	}
	
	clientsConnIFace->SetClientsHandler( this );
	
	boost::shared_ptr< Client > new_session( new Client( metersFactory->CreateCollector(), io_service ) );
	try
	{
		acceptor->async_accept( new_session->socket, boost::bind( &ClientsHandler_Asio::handle_accept, this, new_session, asio::placeholders::error ) );
	}
	catch( std::exception& e )
	{
		Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::ClientsHandler_Asio(): acceptor->async_accept() failed with exception \"%s\".\n" ) % e.what() );
	}
	
	boost::thread dispatcherThread( &ClientsHandler_Asio::Run, this );
}

ClientsHandler_Asio::~ClientsHandler_Asio()
{
	delete metersFactory;
	if ( acceptor != NULL )
		delete acceptor;
}

void ClientsHandler_Asio::Run()
{
	/* From commentary to asio's source code:
	
	 * @par Effect of exceptions thrown from handlers
	 *
	 * If an exception is thrown from a handler, the exception is allowed to
	 * propagate through the throwing thread's invocation of run(), run_one(),
	 * poll() or poll_one(). No other threads that are calling any of these
	 * functions are affected. It is then the responsibility of the application to
	 * catch the exception.
	 *
	 * After the exception has been caught, the run(), run_one(), poll() or
	 * poll_one() call may be restarted @em without the need for an intervening
	 * call to reset(). This allows the thread to rejoin the io_service object's
	 * thread pool without impacting any other threads in the pool.
	
	*/
	for( ;; )
	{
		try
		{
			Globals::messenger->write( boost::format( "I: ClientsHandler_Asio::Run(): running...\n" ) );
			io_service.run();
			Globals::messenger->write( boost::format( "I: ClientsHandler_Asio::Run(): io_service.run() successfully ended.\n" ) );
			break;
		}
		catch ( std::exception& e )
		{
			Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::Run(): io_service.run() failed with exception: \"%s\". Rerunning...\n" ) % e.what() );
		}
	}
}

void ClientsHandler_Asio::DispatchedSend_Single( const uint64_t sessionId, boost::shared_ptr< Transmitter >& transmitter )
{
	clientsMutex.lock();
	
	std::map< uint64_t, boost::shared_ptr< Client > >::iterator it = clients.find( sessionId );
	if ( it != clients.end() )
	{
		boost::shared_ptr< Client >& client = it->second;
		
		asio::async_write( client->socket, asio::buffer( transmitter->GetData(), transmitter->GetLength()), boost::bind( &ClientsHandler_Asio::handle_write, this, client, transmitter, asio::placeholders::error, asio::placeholders::bytes_transferred ) );
	}
	else
	{
		//Globals::messenger->write(boost::format("W: ClientsHandler_Asio::Send(OutStream): client with session id = %llu was NOT found.\n") % sessionId);
	}
	
	clientsMutex.unlock();
}

void ClientsHandler_Asio::DispatchedSend_Array( const std::vector< uint64_t > ids, boost::shared_ptr< Transmitter >& transmitter )
{
	clientsMutex.lock();
	
	bool wasSourceDataChunkPassed = false;
	
	const size_t size = ids.size();
	for ( size_t i = 0; i < size; ++i )
	{
		std::map< uint64_t, boost::shared_ptr< Client > >::iterator it = clients.find( ids[ i ] );
		if ( it != clients.end() )
		{
			boost::shared_ptr< Client >& client = it->second;
			
			asio::async_write( client->socket, asio::buffer( transmitter->GetData(), transmitter->GetLength() ), boost::bind( &ClientsHandler_Asio::handle_write, this, client, transmitter, asio::placeholders::error, asio::placeholders::bytes_transferred ) );
		}
		else
		{
			//Globals::messenger->write(boost::format("W: ClientsHandler_Asio::Send(OutStream): client with session id = %llu was NOT found.\n") % sessionId);
		}
	}
	
	clientsMutex.unlock();
}

void ClientsHandler_Asio::DispatchedDisconnect_Single( const uint64_t sessionId )
{
	clientsMutex.lock();
	
	Disconnect_TU( sessionId );
	
	clientsMutex.unlock();
}

void ClientsHandler_Asio::DispatchedDisconnect_Array( const std::vector< uint64_t > ids )
{
	clientsMutex.lock();
	
	const int size = ids.size();
	for ( int i = 0; i < size; ++i )
	{
		Disconnect_TU( ids[ i ] );
	}
	
	clientsMutex.unlock();
}

void ClientsHandler_Asio::handle_write( boost::shared_ptr< Client >& client, boost::shared_ptr< Transmitter >& transmitter, const asio::error_code& error, const size_t bytes_transferred )
{
	if ( cancelling )
	{
		Globals::messenger->write( boost::format( "I: ClientsHandler_Asio::handle_write(): cancelling - returning.\n" ) );
		return;
	}
	
	if ( error.value() == 0 )
	{
		if ( transmitter->GetLength() != bytes_transferred )
		{
			Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::handle_write(): sent %u bytes but was transferred just %u for client %llu.\n" ) % transmitter->GetLength() % bytes_transferred % client->sessionId );
		}
	}
	else
	{
		//Globals::messenger->write(boost::format("W: ClientsHandler_Asio::handle_write(): writing failed for client %llu. Possible disconnection.\n") % client->sessionId);
	}
}

void ClientsHandler_Asio::Disconnect_TU( const uint64_t& sessionId )
{
	std::map< uint64_t, boost::shared_ptr< Client > >::iterator it = clients.find( sessionId );
	if ( it != clients.end() )
	{
		boost::shared_ptr< Client >& client = it->second;
		
		//client was dropped by server's initiative, that's why in this case клиент отключён по инициативе сервера, поэтому в данном случае закрываем соединение:
		
		asio::error_code shutdownErrorCode;
		asio::error_code closeErrorCode;
		try
		{
			client->socket.shutdown( asio::socket_base::shutdown_both, shutdownErrorCode );
			client->socket.close( closeErrorCode );
		}
		catch( std::exception& e )
		{
			Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::Disconnect_TU(): \"socket.shutdown()\" or \"socket.close()\" failed with exception \"%s\". For an error code read next message (if there will be some).\n" ) % e.what() );
		}
		if ( shutdownErrorCode )
			Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::Disconnect_TU(): socket shutdown failed with \"%s\".\n" ) % shutdownErrorCode.message() );
		if ( closeErrorCode )
			Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::Disconnect_TU(): socket close failed with \"%s\".\n" ) % closeErrorCode.message() );
		
		
		//client will be removed during his disconnection processig within handle_read(), that's why it's not need to delete him here:
		//delete client;
		clients.erase( it );
	}
}

void ClientsHandler_Asio::Send( const Outcoming& outcoming )
{
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedSend_Array, this, outcoming.recipients, boost::shared_ptr< Transmitter >( metersFactory->CreateTransmitter( outcoming.data.c_str(), outcoming.data.size() ) ) ) );
}

void ClientsHandler_Asio::Send( const uint64_t& sessionId, const OutStream& outStream )
{
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedSend_Single, this, sessionId, boost::shared_ptr< Transmitter >( metersFactory->CreateTransmitter( outStream.GetData(), outStream.GetSize() ) ) ) );
}

void ClientsHandler_Asio::Send( const std::vector< uint64_t >& ids, const OutStream& outStream )
{
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedSend_Array, this, ids, boost::shared_ptr< Transmitter >( metersFactory->CreateTransmitter( outStream.GetData(), outStream.GetSize() ) ) ) );
}

void ClientsHandler_Asio::Send( const uint64_t& sessionId, const std::string& data )
{
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedSend_Single, this, sessionId, boost::shared_ptr< Transmitter >( metersFactory->CreateTransmitter( data.c_str(), data.size() ) ) ) );
}

void ClientsHandler_Asio::Send( const uint64_t& sessionId, const void* data, const size_t length )
{
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedSend_Single, this, sessionId, boost::shared_ptr< Transmitter >( metersFactory->CreateTransmitter( data, length ) ) ) );
}

void ClientsHandler_Asio::Disconnect( const uint64_t& sessionId )
{
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedDisconnect_Single, this, sessionId ) );
}

void ClientsHandler_Asio::Disconnect( const std::vector< uint64_t >& ids )
{
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedDisconnect_Array, this, ids ) );
}

boost::shared_ptr< std::vector< uint64_t > > ClientsHandler_Asio::GetIds()
{
	boost::shared_ptr< std::vector< uint64_t > > ids = boost::make_shared< std::vector< uint64_t > >();
	
	clientsMutex.lock();
	
	for ( std::map< uint64_t, boost::shared_ptr< Client > >::iterator it = clients.begin(); it != clients.end(); ++it )
		ids->push_back( it->first );
	
	clientsMutex.unlock();
	
	return ids;
}

void ClientsHandler_Asio::CancelAcceptingConnections()
{
	cancelling = true;
	
	cancellingAcceptingConnectionsContextLock = new ContextLock;
	
	strand.dispatch( boost::bind( &ClientsHandler_Asio::DispatchedCancelAcceptingConnections, this ) );
	
	//wait till function will do it's job:
	cancellingAcceptingConnectionsContextLock->Stop();
}

void ClientsHandler_Asio::DispatchedCancelAcceptingConnections()
{
	acceptor->cancel();
	
	cancellingAcceptingConnectionsContextLock->Resume();
}

void ClientsHandler_Asio::DisconnectClients()
{
	clientsMutex.lock();
	
	while ( !clients.empty() )
	{
		const uint64_t sessionId = clients.begin()->first;
		Disconnect_TU( sessionId );
		clientsConnIFace->HandleDisconnection( sessionId );
	}
	
	clientsMutex.unlock();
}

void ClientsHandler_Asio::handle_accept( boost::shared_ptr< Client >& client, const asio::error_code& error )
{
	if ( cancelling )
	{
		Globals::messenger->write( boost::format( "I: ClientsHandler_Asio::handle_accept(): cancelling - returning.\n" ) );
		return;
	}
	
	if ( error.value() == 0 )
	{
		asio::error_code ec;
		//this call, sometimes, ends with error "Transport endpoint is not connected":
		asio::ip::tcp::endpoint endpoint = client->socket.remote_endpoint( ec );
		if( ec )
		{
			//we don't use raw pointers anymore:
			//delete client;
		}
		else
		{
			const uint64_t sessionId = clientsConnIFace->HandleConnection( endpoint.address().to_string() );
			client->sessionId = sessionId;
			
			clientsMutex.lock();
			if ( clients.insert( std::pair< uint64_t, boost::shared_ptr< Client > >( sessionId, client )).second == false )
				Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::handle_accept(): client was NOT inserted under session id = %llu.\n" ) % sessionId );
			clientsMutex.unlock();
			
			client->socket.async_read_some( asio::buffer( client->readBuffer, Client::BUFFER_SIZE ), boost::bind( &ClientsHandler_Asio::handle_read, this, client, asio::placeholders::error, asio::placeholders::bytes_transferred ) );
		}
	}
	else
	{
		Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::handle_accept(): client session acception failed with error: \"%s\"! Keep accepting connections...\n" ) % error.message() );
	}
	
	//keeping accepting incoming connections:
	client = boost::shared_ptr< Client >( new Client( metersFactory->CreateCollector(), io_service ) );
	try
	{
		acceptor->async_accept( client->socket, boost::bind( &ClientsHandler_Asio::handle_accept, this, client, asio::placeholders::error ) );
	}
	catch( std::exception& e )
	{
		Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::handle_accept(): acceptor->async_accept() failed with exception \"%s\".\n" ) % e.what() );
	}
}

void ClientsHandler_Asio::Drop( boost::shared_ptr< Client >& client, const bool printWarning )
{
	//ignoring if - now, user will be removed only from asio's side, that's why we need this event:
	/*//error "asio::error::operation_aborted" is thrown in case of incorrect socket closing from server side and it happenes from io_service::run()'s thread within strand and Client is already removed (disconnection is properly handled):
	if ( error.value() != asio::error::operation_aborted )
	{*/
		//Globals::messenger->write( boost::format( "I: ClientsHandler_Asio::handle_read(): ordinary client-side disconnection for %llu.\n" ) % client->sessionId);
		
		clientsConnIFace->HandleDisconnection( client->sessionId );
		
		//removing disconnected client:
		clientsMutex.lock();
		if ( clients.erase( client->sessionId ) != 1 )
		{
			//but here "if" is needed, because from array client is removed by server's request (from logic's code/part):
			if ( printWarning )
				Globals::messenger->write( boost::format( "W: ClientsHandler_Asio::handle_read(): client with session id = %llu was NOT removed.\n" ) % client->sessionId );
		}
		clientsMutex.unlock();
	//}
}

class TempTransmitter: public Transmitter
{
public:
	TempTransmitter( void* data, const size_t length ): _data( data ), _length( length )
	{
	}
	
	virtual ~TempTransmitter()
	{
		free( _data );
	}
	
	const void* GetData() const
	{
		return _data;
	}
	
	const size_t GetLength() const
	{
		return _length;
	}

private:
	void* _data;
	size_t _length;
};

void ClientsHandler_Asio::handle_read( boost::shared_ptr< Client >& client, const asio::error_code& error, const size_t bytes_transferred )
{
	if ( cancelling )
	{
		//happens, maybe, with all clients during server's stopping:
		//Globals::messenger->write( boost::format( "I: ClientsHandler_Asio::handle_read(): cancelling - returning.\n" ) );
		return;
	}
	
	if ( error.value() == 0 )
	{
		AsioIncomingDataHandler asioIncomingDataHandler( client->sessionId, clientsConnIFace );
		CollectingResult collectingResult = client->collector->CollectData( bytes_transferred, client->readBuffer, &asioIncomingDataHandler );
		if ( collectingResult._remove )
		{
			Drop( client, false );
			return;
		}
		else if ( collectingResult._response != NULL )
		{
			boost::shared_ptr< Transmitter > transmitter( new TempTransmitter( collectingResult._response, collectingResult._responseLength ) );
			asio::async_write( client->socket, asio::buffer( transmitter->GetData(), transmitter->GetLength()), boost::bind( &ClientsHandler_Asio::handle_write, this, client, transmitter, asio::placeholders::error, asio::placeholders::bytes_transferred ) );
		}
		
		//keeping reading:
		try
		{
			client->socket.async_read_some( asio::buffer( client->readBuffer, Client::BUFFER_SIZE ), boost::bind( &ClientsHandler_Asio::handle_read, this, client, asio::placeholders::error, asio::placeholders::bytes_transferred ) );
		}
		catch( std::exception& e )
		{
			Globals::messenger->write( boost::format( "E: ClientsHandler_Asio::handle_read(): socket.async_read_some() failed with exception \"%s\".\n" ) % e.what() );
		}
	}
	else
	{
		Drop( client, error.value() != asio::error::operation_aborted );
	}
}

}//namespace fos

#include "../../memdbg_end.h"


