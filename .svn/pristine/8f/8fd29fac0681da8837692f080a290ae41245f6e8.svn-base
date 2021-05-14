

#include "mongoose_batch.h"

static void *mongoose_callback(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info)
{
	if (event == MG_NEW_REQUEST)
	{
		Globals::messenger->write(boost::format("I: mongoose_callback(): incoming HTTP request: \"%s\".\n") % request_info->uri);
		
		// Echo requested URI back to the client
		mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", request_info->uri);
		return (void*)(const_cast<char*>(""));  // Mark as processed
	}
	else
	{
		return NULL;
	}
}


void MongooseBatch::OnCreation(const fos::BatchId* batchId, fos::BatchTime* batchTime)
{
	const char *options[] = {"listening_ports", "8081", NULL};
	mgContext = mg_start(mongoose_callback, this, options);
	
	commonTime = batchTime->AllocateCommonTime();
}

void MongooseBatch::OnDestruction(fos::BatchTime* batchTime)
{
	Globals::messenger->write(boost::format("I: MongooseBatch::OnDestruction(): stopping HTTP server...\n"));
	mg_stop(mgContext);
	Globals::messenger->write(boost::format("I: MongooseBatch::OnDestruction(): HTTP server stopped.\n"));
	
	delete commonTime;
}


