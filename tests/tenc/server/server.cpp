
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <asio.hpp>
#include <stdexcept>
#include <boost/cstdint.hpp>

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
using boost::uint64_t;
#endif

uint64_t lastSessionId = 1;

class ClientSession
{
public:
	ClientSession(asio::io_service& io_service, const uint64_t& sessionId): socket(io_service), sessionId(sessionId)
	{
	}
	
	asio::ip::tcp::socket socket;
	uint64_t sessionId;
	
	static const int BUFFER_SIZE = 128;
	int readBuffer[BUFFER_SIZE];
};

class Server
{
public:
	
	Server(const short port);
	
private:
	
	void Run();
	
	void handle_accept(ClientSession* clientSession, const asio::error_code& error);
	
	void handle_read(ClientSession* clientSession, const asio::error_code& error, size_t bytes_transferred);
	void handle_write(ClientSession* clientSession, boost::shared_ptr<std::string> data, const asio::error_code& error, size_t bytes_transferred);
	
	asio::io_service io_service;
	asio::ip::tcp::acceptor acceptor;
	asio::strand strand;
};

Server::Server(const short port): acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), strand(io_service)
{
	ClientSession* new_session = new ClientSession(io_service, lastSessionId);
	try
	{
		acceptor.async_accept(new_session->socket, boost::bind(&Server::handle_accept, this, new_session, asio::placeholders::error));
	}
	catch(std::exception& e)
	{
		printf("acceptor.async_accept() failed with exception \"%s\".\n", e.what());
	}
	
	boost::thread runningThread(&Server::Run, this);
	
	printf("waiting for incoming connections...\n");
}

void Server::Run()
{
	/* Comment from io_service.hpp:
	
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
	for(;;)
	{
		try
		{
			asio::error_code ec;
			printf("running...\n");
			io_service.run(ec);
			if(ec.value() != 0)
			{
				printf("io_service.run() failed with error \"%s\".\n", ec.message());
				continue;
			}
			else
			{
				printf("io_service.run() successfully ended.\n");
			}
			break;
		}
		catch (std::exception& e)
		{
			printf("io_service.run() failed with exception \"%s\".\n", e.what());
		}
	}
}

void Server::handle_write(ClientSession* clientSession, boost::shared_ptr<std::string> data, const asio::error_code& error, size_t bytes_transferred)
{
	if(error.value() == 0)
	{
		if(data->size() != bytes_transferred)
		{
			printf("ERROR: sent %d bytes but was transferred just %d for client %lld.\n", data->size(), bytes_transferred, clientSession->sessionId);
		}
		else
		{
			printf("data of length %d sent.\n", bytes_transferred);
		}
	}
	else
	{
		printf("WARNING: writing failed for client %lld. Possible disconnection.\n", clientSession->sessionId);
	}
}

void Server::handle_accept(ClientSession* clientSession, const asio::error_code& error)
{
	if (error.value() == 0)
	{
		clientSession->socket.async_read_some(asio::buffer(clientSession->readBuffer, ClientSession::BUFFER_SIZE), boost::bind(&Server::handle_read, this, clientSession, asio::placeholders::error, asio::placeholders::bytes_transferred));
		
		lastSessionId++;
		clientSession = new ClientSession(io_service, lastSessionId);
		try
		{
			acceptor.async_accept(clientSession->socket, boost::bind(&Server::handle_accept, this, clientSession, asio::placeholders::error));
		}
		catch(std::exception& e)
		{
			printf("ERROR: handle_accept(): acceptor.async_accept() failed with exception \"%s\".\n", e.what());
		}
	}
	else
	{
		printf("ERROR: client session acception failed! Don't even know what to do...\n");
		delete clientSession;
	}
}

void Server::handle_read(ClientSession* clientSession, const asio::error_code& error, size_t bytes_transferred)
{
	if (error.value() == 0)
	{
		printf("incoming data of length %d.\n", bytes_transferred);
		
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			boost::shared_ptr<std::string> data = boost::make_shared<std::string>();
			data->push_back(123);
			printf("sending...\n");
			clientSession->socket.async_write_some(asio::buffer(*data), boost::bind(&Server::handle_write, this, clientSession, data, asio::placeholders::error, asio::placeholders::bytes_transferred));
		}
		
		//continue reading:
		try
		{
			clientSession->socket.async_read_some(asio::buffer(clientSession->readBuffer, ClientSession::BUFFER_SIZE), boost::bind(&Server::handle_read, this, clientSession, asio::placeholders::error, asio::placeholders::bytes_transferred));
		}
		catch(std::exception& e)
		{
			printf("ERROR: Server::handle_read(): socket.async_read_some() failed with exception \"%s\".\n", e.what());
		}
	}
	else
	{
		//asio::error::operation_abort error is set in case of correct socket close from server side, so don't remove already removed client in that case:
		if(error.value() != asio::error::operation_aborted)
		{
			delete clientSession;
		}
	}
}

int main()
{
	Server server(4670);
	
	getchar();
}


