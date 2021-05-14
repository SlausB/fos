

#include "my_meters_factory.h"

#include "fos/src/handlers/meter/leb128_collector.h"
#include "fos/src/handlers/meter/simple_transmitter.h"


Collector* MyMetersFactory::CreateCollector()
{
	return new LEB128Collector;
}

Transmitter* MyMetersFactory::CreateTransmitter(const void* data, const size_t length)
{
	return new SimpleTransmitter(data, length);
}


