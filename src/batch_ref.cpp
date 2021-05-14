
#include "batch_ref.h"

#include "memdbg_start.h"

namespace fos
{

BatchRef::BatchRef(): last( false )
{
}

BatchRef::BatchRef( const BatchRef& batchRef )
{
	Copy( batchRef );
}

BatchRef& BatchRef::operator=( const BatchRef& batchRef )
{
	Copy( batchRef );
	return *this;
}


BatchRef::~BatchRef()
{
	if( last )
		destructionHandler( opaqueData );
}

BatchRef::BatchRef( Batch* batch, DestructionHandler destructionHandler, void* opaqueData ): batch( batch ), destructionHandler( destructionHandler ), opaqueData( opaqueData )
{
	last = true;
}

void BatchRef::Copy( const BatchRef& batchRef )
{
	last = batchRef.last;
	batch = batchRef.batch;
	
	destructionHandler = batchRef.destructionHandler;
	opaqueData = batchRef.opaqueData;
	
	( ( BatchRef& ) batchRef ).last = false;
}

}//namespace scaserv

#include "memdbg_end.h"



