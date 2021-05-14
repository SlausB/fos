

#ifndef MY_METERS_FACTORY_H
#define MY_METERS_FACTORY_H

#include "fos/src/fos.h"

class MyMetersFactory: public fos::MetersFactory
{
public:
	Collector* CreateCollector();
	Transmitter* CreateTransmitter(const void* data, const size_t length);
};

#endif//#ifndef MY_METERS_FACTORY_H


