
#include <asio.hpp>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

asio::io_service io_service;
asio::ip::tcp::socket _socket(io_service);
asio::strand strand(io_service);

void ServiceRunner()
{
	io_service.run();
}

void WriteHandler(boost::shared_ptr<std::string> data)
{
	printf("sent\n");
}

void Send()
{
	printf("sending...\n");
	boost::shared_ptr<std::string> data = boost::make_shared<std::string>();
	data->push_back(123);
	_socket.async_write_some(asio::buffer(*data), boost::bind(WriteHandler, data));
}

int main()
{
	try
	{	
		asio::ip::tcp::resolver resolver(io_service);
		asio::ip::tcp::resolver::query query("localhost", "4670");
		asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		asio::ip::tcp::resolver::iterator end;
		asio::error_code error = asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			_socket.close();
			printf("connecting...\n");
			_socket.connect(*endpoint_iterator++, error);
		}
		
		if (error)
		{
			printf("error: \"%s\".\n", error.message().c_str());
		}
		else
		{
			strand.dispatch(Send);
		}

		boost::thread serviceThread(ServiceRunner);
	}
	catch (std::exception& e)
	{
		printf("Connection failed with exception \"%s\".\n", e.what());
	}

	getchar();

	return 0;
}

