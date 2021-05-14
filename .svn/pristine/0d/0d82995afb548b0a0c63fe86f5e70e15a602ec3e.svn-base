
#include "clients_conn_iface.h"

#include "dispatcher.h"

#include "handlers/clients_handler.h"

#include "memdbg_start.h"


namespace fos
{


ClientsConnIFace::ClientsConnIFace(Dispatcher* dispatcher): dispatcher(dispatcher)
{
}

ClientsConnIFace::~ClientsConnIFace()
{
	delete clientsHandler;
}

void ClientsConnIFace::SetClientsHandler(ClientsHandler* clientsHandler)
{
	this->clientsHandler = clientsHandler;
}

const uint64_t& ClientsConnIFace::HandleConnection(const std::string& ipAddress)
{
	return dispatcher->HandleConnection(ipAddress);
}

const uint64_t& ClientsConnIFace::HandleConnection(const char* ipAddress)
{
	return dispatcher->HandleConnection(ipAddress);
}

void ClientsConnIFace::HandleMessage(const uint64_t& sessionId, const size_t dataLength, const char* messageData)
{
	dispatcher->HandleMessage(sessionId, dataLength, messageData);
}

void ClientsConnIFace::HandleDisconnection(const uint64_t& sessionId)
{
	dispatcher->HandleDisconnection(sessionId);
}

void ClientsConnIFace::Send(const Outcoming& outcoming)
{
	clientsHandler->Send(outcoming);
}

void ClientsConnIFace::Send(const uint64_t& id, const OutStream& outStream)
{
	clientsHandler->Send(id, outStream);
}

void ClientsConnIFace::Send(const std::vector<uint64_t>& ids, const OutStream& outStream)
{
	clientsHandler->Send(ids, outStream);
}

void ClientsConnIFace::Send(const uint64_t& id, const std::string& data)
{
	clientsHandler->Send(id, data);
}

void ClientsConnIFace::Send(const uint64_t& id, const void* data, const size_t length)
{
	clientsHandler->Send(id, data, length);
}

void ClientsConnIFace::Disconnect(const uint64_t& id)
{
	clientsHandler->Disconnect(id);
}

void ClientsConnIFace::Disconnect(const std::vector<uint64_t>& ids)
{
	clientsHandler->Disconnect(ids);
}

boost::shared_ptr<std::vector<uint64_t> > ClientsConnIFace::GetIds()
{
	return clientsHandler->GetIds();
}

void ClientsConnIFace::CancelAcceptingConnections()
{
	clientsHandler->CancelAcceptingConnections();
}

void ClientsConnIFace::DisconnectClients()
{
	clientsHandler->DisconnectClients();
}

}//namespace fos

#include "memdbg_end.h"

