
#include "db_iface.h"

#include "boost/interprocess/sync/scoped_lock.hpp"


boost::mutex db::ResponseRef::deletionMutex;


int db::CheckParameters(Messenger* messenger, db::Response *response, char *context, int resultsCount)
{
	const int resultsCountInPractice = response->GetResultsCount();
	if(resultsCountInPractice < resultsCount)
	{
		messenger->write(boost::format("E: for \"%s\" must be %d results but it's only %d of them. Something wrong.\n") % context % resultsCount % resultsCountInPractice);
		return 1;
	}
	return 0;
}

void db::Interface::Exit()
{
}


db::Response::~Response()
{
}



db::ResponseRef::ResponseRef(): response( NULL )
{
}

db::ResponseRef::ResponseRef( db::Response* response ): response( response )
{
	if ( response != NULL )
		references = new int(1);
}

db::ResponseRef::~ResponseRef()
{
	deletionMutex.lock();
	
	if ( response != NULL )
	{
		( *references )--;
		if( ( *references ) <= 0 )
		{
			delete response;
			delete references;
		}
	}
	
	deletionMutex.unlock();
}

db::ResponseRef::ResponseRef( const db::ResponseRef& responseRef ): response( NULL )
{
	Copy(responseRef);
}

db::ResponseRef& db::ResponseRef::operator=(const db::ResponseRef& responseRef)
{
	Copy(responseRef);
	return *this;
}

int db::ResponseRef::GetResultsCount()
{
	if(response == NULL) return 0;
	return response->GetResultsCount();
}

db::Result* db::ResponseRef::GetResult(int index)
{
	if(response == NULL) return NULL;
	return response->GetResult(index);
}

void db::ResponseRef::Print(Messenger *messenger)
{
	if(response == NULL) return;
	response->Print(messenger);
}

bool db::ResponseRef::IsOk()
{
	if(response == NULL) return false;
	return response->IsOk();
}

void db::ResponseRef::Copy(const db::ResponseRef& responseRef)
{
	deletionMutex.lock();
	
	if(responseRef.response == NULL)
	{
		//копируем нулевую "ссылку"?
		if(response != NULL)
		{
			(*references)--;
			if((*references) <= 0)
			{
				delete response;
				delete references;
			}
			
			response = NULL;
		}
	}
	else
	{
		references = responseRef.references;
		response = responseRef.response;
		(*references)++;
	}
	
	deletionMutex.unlock();
}


bool db::Interface::PassFilter(const char* msg)
{
	boost::interprocess::scoped_lock<boost::mutex> lock(filtersMutex);
	
	for(std::vector<std::string>::iterator it = filters.begin(); it != filters.end(); it++)
	{
		if(strstr(msg, it->c_str()) != NULL)
		{
			return false;
		}
	}
	
	return true;
}

void db::Interface::AddFilter(const char* msg)
{
	boost::interprocess::scoped_lock<boost::mutex> lock(filtersMutex);
	
	//сначала проверяем что такого фильтра ещё нет:
	for(std::vector<std::string>::iterator it = filters.begin(); it != filters.end(); it++)
	{
		if(it->compare(msg) == 0)
		{
			return;
		}
	}
	
	filters.push_back(msg);
}

void db::Interface::RemoveFilter(const char* msg)
{
	boost::interprocess::scoped_lock<boost::mutex> lock(filtersMutex);
	
	for(std::vector<std::string>::iterator it = filters.begin(); it != filters.end(); it++)
	{
		if(it->compare(msg) == 0)
		{
			filters.erase(it);
			return;
		}
	}
}



