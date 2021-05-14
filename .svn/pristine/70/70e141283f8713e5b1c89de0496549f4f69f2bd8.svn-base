

#ifndef FOS_METERS_FACTORY_H
#define FOS_METERS_FACTORY_H

#include "handlers/meter/collector.h"
#include "handlers/meter/transmitter.h"

namespace fos
{

/** Фабрика призвана создавать трансмиттеры и коллекторы для определения политики (правила) передачи-получения данных.*/
class MetersFactory
{
public:
	virtual Collector* CreateCollector() = 0;
	virtual Transmitter* CreateTransmitter(const void* data, const size_t length) = 0;
};

}//namespace fos

#endif//#ifndef FOS_METERS_FACTORY_H

