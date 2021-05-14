
#include <asio.hpp>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

asio::io_service* io_service = new asio::io_service;
asio::ip::tcp::socket* _socket = new asio::ip::tcp::socket(*io_service);
asio::strand* strand = new asio::strand(*io_service);

const int READ_BUFFER_SIZE = 128;
char readBuffer[READ_BUFFER_SIZE];

void ServiceRunner()
{
	io_service->run();
}

void handle_read(asio::error_code ec, size_t bytes_transferred)
{
	printf("%d bytes sent.\n", bytes_transferred);
}

void handle_write(boost::shared_ptr<std::string> data)
{
	printf("sent\n");
}

void Send()
{
	printf("sending...\n");
	boost::shared_ptr<std::string> data = boost::make_shared<std::string>();
	data->push_back(123);
	_socket->async_write_some(asio::buffer(*data), boost::bind(handle_write, data));
}

int main()
{
	try
	{	
		asio::ip::tcp::resolver resolver(*io_service);
		asio::ip::tcp::resolver::query query("localhost", "4670");
		asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		asio::ip::tcp::resolver::iterator end;
		asio::error_code error = asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			_socket->close();
			printf("connecting...\n");
			_socket->connect(*endpoint_iterator++, error);
		}
		
		if (error)
		{
			printf("error: \"%s\".\n", error.message().c_str());
		}
		else
		{
			_socket->async_read_some(asio::buffer(readBuffer, READ_BUFFER_SIZE), boost::bind(handle_read, asio::placeholders::error, asio::placeholders::bytes_transferred));
			strand->dispatch(Send);

			boost::thread serviceThread(ServiceRunner);
			
			//hope Send will be finished:
			boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		}
	}
	catch (std::exception& e)
	{
		printf("Connection failed with exception \"%s\".\n", e.what());
	}

	return 0;
}

