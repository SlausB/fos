
using boost::asio::ip::tcp;

void RunClient(const int threadIndex)
{

	try
	{
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(ADDRESS, PORT);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;
		tcp::socket socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}

		if (error)
		{
			throw boost::system::system_error(error);
		}
		else
		{
			messenger.write("I: connection to server succeeded\n");
		}

		bool someError = false;

		//HELLO:
		if(!someError)
		{
			//writing:

			OutStream outStream;
			int32_t helloMsgType = ClientToServer::HELLO;
			outStream << helloMsgType;
			boost::system::error_code writeError;
			socket.write_some(boost::asio::buffer(outStream.GetData()), writeError);
			if(writeError)
			{
				messenger << boost::format("E: sending \"HELLO\" failed with \"%s\".\n") % writeError.message();
				someError = true;
			}
			else
			{
				messenger << "I: \"HELLO\" sent.\n";
			}


			//reading:

			std::string inBuf;
			bool readError = false;
			utils::Read(inBuf, socket, readError);
			if(readError)
			{
				messenger << "E: ERROR: reading \"HELLO\" message failed.\n";
				someError = true;
			}
			InStream inStream(inBuf);
			int32_t inMsgType;
			READ(inStream >> inMsgType);
			if(inMsgType == ServerToClient::HELLO)
			{
				messenger << "I: OK: \"HELLO\" got.\n";
			}
			else
			{
				messenger << boost::format("E: ERROR: got %d instead of \"HELLO\".\n");
				someError = true;
			}

			if(!someError)
			{
				const int leftBytes = inStream.LeftBytes();
				if(leftBytes != 0)
				{
					messenger << boost::format(boost::format("E: ERROR: left %d some bytes after \"HELLO\" reading.\n") % leftBytes);
				}
			}
		}

		if(!someError)
		{

		}
	}
	catch (std::exception& e)
	{
		messenger.error(e.what());
		messenger.endl();
	}
}
